#ifndef MOVEROWSDIALOG_H
#define MOVEROWSDIALOG_H

#include <QDialog>
#include  "qstditemmodel_commands.h"
class QStdItemModel;
class QIntValidator;
namespace Ui {
class MoveRowsDialog;
}

class MoveRowsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoveRowsDialog(QStdItemModel*,QWidget *parent = nullptr);
    ~MoveRowsDialog();
    std::unique_ptr<QStdItemModel::move_rows_cmd_t> get_cmd();
private:
     std::unique_ptr<QStdItemModel::move_rows_cmd_t>  cmd;

    QIntValidator* src_row_validator;
    QIntValidator* count_validator;
    QIntValidator* dest_child_validator;
    QIntValidator* src_parent_validator;
    QIntValidator* dest_parent_validator;

     QModelIndex m_sourceParent;
    int m_sourceRow;
    int m_count;
      QModelIndex m_destinationParent;
     int m_destinationChild;


    QStdItemModel* m_model;
    Ui::MoveRowsDialog *ui;
private slots:

    void on_src_parent(const QString&);

    void on_dest_parent(const QString&);

    void on_src_row(const QString&);
    void on_dest_child(const QString&);
    void on_count(const QString&);

    void on_ok();

};

#endif // MOVEROWSDIALOG_H
