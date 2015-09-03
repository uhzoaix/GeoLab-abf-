#ifndef GCL_GUI_SCENEGRAPHITEMMODEL_H
#define GCL_GUI_SCENEGRAPHITEMMODEL_H
#include "Core/gcl_global.h"
#include <QAbstractItemModel>
namespace GCL {
namespace SceneGraph {
class SceneGraphNode;
}}
namespace GCL {
namespace GUI {

class GCL_GUI_SHARED_EXPORT SceneGraphItemModel : public QAbstractItemModel {

  Q_OBJECT

public:
  explicit SceneGraphItemModel(SceneGraph::SceneGraphNode *  _scenegraph);
  virtual ~SceneGraphItemModel();

  enum Column {
    COL_NAME = 0,
    COL_VISIBLE = 1,
    COL_SELECTED = 2,
    COL_TYPE = 3
  };



  //// read only model access functions inherited from QAbstractItemModel
  virtual QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const;
  virtual Qt::ItemFlags flags(const QModelIndex& _index) const;
  virtual QVariant data(const QModelIndex& _index, int _role) const;
  virtual QModelIndex index(int _row, int _column, const QModelIndex& _parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& _index) const;
  virtual int rowCount(const QModelIndex& _parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex& _parent = QModelIndex()) const;

  bool hasChildren(const QModelIndex &parent) const;
  //// layout changeing functions inherited from QAbstractItemModel
  virtual bool removeRow(int _row, const QModelIndex& _parent = QModelIndex());


  //// editable model access functions inherited from QAbstractItemModel
  virtual bool setData (const QModelIndex& _index, const QVariant& _value, int _role);



private:
  SceneGraph::SceneGraphNode* scenegraph_;
};
}}
#endif // SCENEGRAPHITEMMODEL_H
