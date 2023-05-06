// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "indexer.h"

#include "program/clangindex.h"

#include <libclang-utils/index-action.h>
#include <libclang-utils/clang-cursor.h>

#include <QRunnable>
#include <QThreadPool>

#include <cassert>
#include <filesystem>
#include <iostream>

namespace clark
{

File* get_file(IndexingResult& idx, std::filesystem::path p)
{
  auto it = idx.files.find(p);

  if (it != idx.files.end())
    return it->second.get();

  auto* f = new File;
  f->path = p.generic_u8string();
  idx.files[p] = std::unique_ptr<File>(f);
  return f;
}

class TranslationUnitIndexer : public libclang::BasicIndexer
{
public:
  IndexingResult result;

public:
  TranslationUnitIndexer(libclang::LibClang& api) : libclang::BasicIndexer(api)
  {

  }

  CXIdxClientContainer startedTranslationUnit()
  {
    return nullptr;
  }

  void* enteredMainFile(const libclang::File& mainFile)
  {
    std::string path = mainFile.getFileName();
    File* f = get_file(result, path);
    return f;
  }

  void* ppIncludedFile(const CXIdxIncludedFileInfo* inclFile)
  {
    std::string path = libclangAPI().file(inclFile->file).getFileName();

    File* f = get_file(result, path);

    FileLocation loc = getFileLocation(inclFile->hashLoc);

    Include inc;
    inc.included_file = f;
    inc.line = loc.line;

    if (loc.client_data)
      inc.file = reinterpret_cast<File*>(loc.client_data);

    if (inc.file)
    {
      result.ppincludes.push_back(inc);
    }
    else
    {
      std::cerr << "could not get id for " << libclangAPI().file(loc.file).getFileName() << std::endl;
    }

    return f;
  }

  void indexDeclaration(const CXIdxDeclInfo* decl)
  {
    FileLocation loc = getFileLocation(decl->loc);

    if (!loc.client_data)
    {
      std::cerr << "indexDeclaration(): unknown file" << std::endl;
      return;
    }

    Entity* symbol = get_entity(decl);

    if (!symbol)
      return;

    setClientData(decl->declAsContainer, symbol);
    setClientData(decl->entityInfo, symbol);

    // It seems indexEntityReference() is not called for declaration despite the 
    // CXSymbolRole enum suggesting it could; so we create the reference manually here.
    if(loc.client_data)
    {
      EntityReference entref;
      entref.file = reinterpret_cast<File*>(loc.client_data);
      entref.col = loc.column;
      entref.line = loc.line;
      entref.symbol = symbol;

      entref.flags = decl->isDefinition ? CXSymbolRole_Definition : CXSymbolRole_Declaration;

      if (decl->isImplicit)
        entref.flags |= CXSymbolRole_Implicit;

      if (void* cdata = getClientData(decl->semanticContainer))
        entref.parent_symbol = reinterpret_cast<Entity*>(cdata);

      result.references.push_back(entref);
    }
  }

  void indexEntityReference(const CXIdxEntityRefInfo* ref)
  {
    FileLocation loc = getFileLocation(ref->loc);

    if (!loc.client_data)
    {
      std::cerr << "indexEntityReference(): unknown file" << std::endl;
      return;
    }

    Entity* symbol = get_entity(ref->referencedEntity);

    if (!symbol)
      return;

    EntityReference symref;
    symref.file = reinterpret_cast<File*>(loc.client_data);
    symref.col = loc.column;
    symref.line = loc.line;
    symref.symbol = symbol;
    symref.flags = ref->role;

    if (ref->parentEntity)
    {
      if (Entity* parent_symbol = get_entity(ref->parentEntity))
        symref.parent_symbol = parent_symbol;
    }

    result.references.push_back(symref);
  }

protected:

  Entity* lookup_symbol(const std::string& usr) const
  {
    auto it = result.symbols.find(usr);
    return it != result.symbols.end() ? it->second.get() : nullptr;
  }

  Entity* get_entity(const CXIdxDeclInfo* decl)
  {
    std::string usr{ decl->entityInfo->USR };

    if (Entity* symbol = lookup_symbol(usr))
    {
      if (decl->isDefinition)
      {
        // we may more accurately fill the Entity struct here

        if (decl->entityInfo->kind == CXIdxEntity_CXXClass)
        {
          list_bases(symbol, decl);
        }

        fill_symbol(*symbol, libclangAPI().cursor(decl->cursor));
      }

      return symbol;
    }
    else
    {
      // The SymbolId was just created, we need to create and fill the corresponding Entity struct.

      std::unique_ptr<Entity> sym = create_symbol(decl);

      if (sym->kind == Whatsit::CXXClass)
      {
        list_bases(*sym, decl);
      }

      fill_symbol(*sym, libclangAPI().cursor(decl->cursor));

      result.symbols[usr] = std::unique_ptr<Entity>(sym.get());

      return sym.release();
    }
  }

  Entity* get_entity(const CXIdxEntityInfo* info)
  {
    if (void* cdata = getClientData(info))
      return reinterpret_cast<Entity*>(cdata);

    std::string usr{ info->USR };

    if (Entity* symbol = lookup_symbol(usr))
    {
      return symbol;
    }
    else
    {
      // The SymbolId was just created, we need to create and fill the corresponding Entity struct.

      std::unique_ptr<Entity> sym = create_symbol(info);
      result.symbols[usr] = std::unique_ptr<Entity>(sym.get());
      return sym.release();
    }
  }

private:

  Entity* get_symbol(const libclang::Cursor& cursor)
  {
    if (cursor.isNull())
      return nullptr;

    static const std::map<CXCursorKind, CXIdxEntityKind> dict = {
      { CXCursor_TypedefDecl,        CXIdxEntity_Typedef },
      { CXCursor_FunctionDecl,       CXIdxEntity_Function },
      { CXCursor_VarDecl,            CXIdxEntity_Variable },
      { CXCursor_EnumConstantDecl,   CXIdxEntity_EnumConstant },
      { CXCursor_EnumDecl,           CXIdxEntity_Enum },
      { CXCursor_StructDecl,         CXIdxEntity_Struct },
      { CXCursor_UnionDecl,          CXIdxEntity_Union },
      { CXCursor_ClassDecl,          CXIdxEntity_CXXClass },
      { CXCursor_Namespace,          CXIdxEntity_CXXNamespace },
      { CXCursor_NamespaceAlias,     CXIdxEntity_CXXNamespaceAlias },
      { CXCursor_Constructor,        CXIdxEntity_CXXConstructor },
      { CXCursor_Destructor,         CXIdxEntity_CXXDestructor },
      { CXCursor_ConversionFunction, CXIdxEntity_CXXConversionFunction },
      { CXCursor_TypeAliasDecl,      CXIdxEntity_CXXTypeAlias },
    };

    auto it = dict.find(cursor.kind());

    if (it == dict.end())
      return nullptr;

    std::string usr{ cursor.getUSR() };

    if (usr.empty())
      return nullptr;

    if (Entity* ent = lookup_symbol(usr))
    {
      return ent;
    }
    else
    {
      // create symbol
      std::unique_ptr<Entity> sym = create_symbol(cursor, static_cast<Whatsit>(it->second));
      result.symbols[usr] = std::unique_ptr<Entity>(sym.get());
      return sym.release();
    }
  }

  Entity* get_parent_symbol(const CXIdxEntityInfo* info)
  {
    libclang::Cursor c = libclangAPI().cursor(info->cursor);
    c = c.getSemanticParent();
    return get_symbol(c);
  }

  const char* name(const CXIdxEntityInfo* entity)
  {
    return entity->name != nullptr ? entity->name : "";
  }

  void fill_symbol(Entity& s, const libclang::Cursor& c)
  {
    switch (c.kind())
    {
    case CXCursor_EnumDecl:
    {
      clark::set_flag(s, Entity::IsScoped, c.EnumDecl_isScoped());
    }
    break;
    case CXCursor_CXXMethod:
    case CXCursor_Constructor:
    case CXCursor_Destructor:
    {
      clark::set_flag(s, Entity::Default, c.CXXMethod_isDefaulted());
      clark::set_flag(s, Entity::Const, c.CXXMethod_isConst());
      clark::set_flag(s, Entity::Static, c.CXXMethod_isStatic());
      clark::set_flag(s, Entity::Virtual, c.CXXMethod_isVirtual());
      clark::set_flag(s, Entity::Pure, c.CXXMethod_isPureVirtual());
    }
    default:
      break;
    }

  }

  std::unique_ptr<Entity> create_symbol(const libclang::Cursor& cursor, Whatsit what, Entity* parent)
  {
    auto s = std::make_unique<Entity>(what, cursor.getSpelling());
    s->display_name = cursor.getDisplayName();
    s->usr = cursor.getUSR();

    fill_symbol(*s, cursor);

    s->parent = parent;

    return s;
  }

  std::unique_ptr<Entity> create_symbol(const libclang::Cursor& cursor, Whatsit what)
  {
    Entity* parent = get_symbol(cursor.getSemanticParent());
    return create_symbol(cursor, what, parent);
  }

  std::unique_ptr<Entity> create_symbol(const CXIdxEntityInfo* info, Entity* parent)
  {
    auto s = std::make_unique<Entity>(static_cast<Whatsit>(info->kind), name(info));
    s->display_name = libclangAPI().cursor(info->cursor).getDisplayName();
    s->usr = info->USR;
    s->parent = parent;

    fill_symbol(*s, libclangAPI().cursor(info->cursor));

    return s;
  }

  std::unique_ptr<Entity> create_symbol(const CXIdxEntityInfo* info)
  {
    Entity* parent = get_parent_symbol(info);
    return create_symbol(info, parent);
  }

  std::unique_ptr<Entity> create_symbol(const CXIdxDeclInfo* decl)
  {
    Entity* parent = nullptr;
    
    if (void* cdata = getClientData(decl->semanticContainer))
      parent = reinterpret_cast<Entity*>(cdata);

    if (parent)
      return create_symbol(decl->entityInfo, parent);
    else
      return create_symbol(decl->entityInfo);
  }

  void list_bases(Entity* entity, const CXIdxDeclInfo* decl)
  {
    const CXIdxCXXClassDeclInfo* classdecl = getCXXClassDeclInfo(decl);

    if (!classdecl)
      return;

    for (unsigned int i(0); i < classdecl->numBases; ++i)
    {
      const CXIdxBaseClassInfo* base = classdecl->bases[i];

      if (void* cdata = getClientData(base->base))
      {
        BaseClass b;
        b.base = reinterpret_cast<Entity*>(cdata);
        b.derived = entity;
        b.access_specifier = static_cast<clark::AccessSpecifier>(libclangAPI().cursor(base->cursor).getCXXAccessSpecifier());
        result.bases.push_back(b);
      }
    }
  }

  void list_bases(Entity& symbol, const CXIdxDeclInfo* decl)
  {
    list_bases(&symbol, decl);
  }
};


IndexingResult index_translation_unit(libclang::Index& index, libclang::TranslationUnit& tunit)
{
  libclang::IndexAction action{ index };

  auto start = std::chrono::high_resolution_clock::now();

  TranslationUnitIndexer tui{ index.api };
  action.indexTranslationUnit(tunit, tui);

  auto end = std::chrono::high_resolution_clock::now();
  tui.result.indexing_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  return std::move(tui.result);
}

} // namespace clark


class IndexTranslationUnit : public QRunnable
{
public:
  TranslationUnitIndexing* indexing = nullptr;

public:
  IndexTranslationUnit(TranslationUnitIndexing* idx) :
    indexing(idx)
  {
  }

  void run() override
  {
    libclang::Index& clangindex = indexing->translationUnit().clangIndex()->libclangIndex();
    libclang::TranslationUnit& tunit = *indexing->translationUnit().clangTranslationUnit();
    clark::IndexingResult ir = clark::index_translation_unit(clangindex, tunit);
    indexing->setIndexingResult(std::move(ir));
  }
};

TranslationUnitIndexing::TranslationUnitIndexing(TranslationUnit& tunit, QObject* parent) : QObject(parent),
  m_translation_unit(tunit)
{

}


TranslationUnitIndexing::State TranslationUnitIndexing::state() const
{
  return m_state;
}

bool TranslationUnitIndexing::isStarted() const
{
  return state() == Started;
}

bool TranslationUnitIndexing::isReady() const
{
  return state() == Ready;
}

TranslationUnit& TranslationUnitIndexing::translationUnit() const
{
  return m_translation_unit;
}

void TranslationUnitIndexing::start()
{
  if (isStarted() || isReady())
    return;

  QThreadPool::globalInstance()->start(new IndexTranslationUnit(this));

  m_state = Started;
  emit started();
}

const clark::IndexingResult& TranslationUnitIndexing::indexingResult() const
{
  static const clark::IndexingResult static_result = {};

  if (!isReady())
    return static_result;
  else
    return m_result;
}

void TranslationUnitIndexing::setIndexingResult(clark::IndexingResult r)
{
  m_result = std::move(r);
  m_state = Ready;
  Q_EMIT ready();
}
