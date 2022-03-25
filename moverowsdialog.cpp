#include "moverowsdialog.h"
#include <QIntValidator>
#include "ui_moverowsdialog.h"
#include "qstditemmodel_p.h"

MoveRowsDialog::MoveRowsDialog(QStdItemModel* m,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveRowsDialog),
    m_model(m)


{
    ui->setupUi(this);

    src_row_validator=new QIntValidator(0,0,this);
    count_validator= new QIntValidator(0,0,this);
    dest_child_validator = new QIntValidator(0,0,this);

    // this dialog is window-model, so while it is active,
    // no new items  can enter the model (what would increase next_free_uuid)
    src_parent_validator= new QIntValidator(0,QStdItemPrivate::next_free_uuid-1,this);
    dest_parent_validator = new QIntValidator(0,QStdItemPrivate::next_free_uuid-1,this);

    ui->lineEdit_src_parent_uid->setValidator(src_parent_validator);
        ui->lineEdit_dest_parent_uid->setValidator(dest_parent_validator);


    ui->lineEdit_count->setValidator(count_validator);
    ui->lineEdit_dest_child->setValidator(dest_child_validator);
    ui->lineEdit_src_row->setValidator(src_row_validator);



        connect(ui->lineEdit_dest_parent_uid,&QLineEdit::textEdited,
                this,&MoveRowsDialog::on_dest_parent);
        connect(ui->lineEdit_src_parent_uid,&QLineEdit::textEdited,
                this,&MoveRowsDialog::on_src_parent);

        connect(ui->lineEdit_src_row,&QLineEdit::textEdited,
                this,&MoveRowsDialog::on_src_row);

        connect(ui->lineEdit_dest_child,&QLineEdit::textEdited,
                this,&MoveRowsDialog::on_dest_child);

        connect(ui->lineEdit_count,&QLineEdit::textEdited,
                this,&MoveRowsDialog::on_count);

           connect(ui->buttonBox, &QDialogButtonBox::accepted, this,&MoveRowsDialog::on_ok);
}

void MoveRowsDialog::on_dest_child(const QString& text)
{
    auto _text{text};
    auto child{text.toInt()};

    int pos{0};
    auto state{dest_child_validator->validate(_text,pos) };

   if( QValidator::Acceptable == state)
    {

       m_destinationChild=child;

        return;
    }

   if(QValidator::Intermediate==state            )
   {
       return;
   }

   ui->lineEdit_dest_child->clear();

}


void MoveRowsDialog::on_src_row(const QString& text)
{
    auto _text{text};
    auto row{text.toInt()};
    int pos{0};
    auto state{src_row_validator->validate(_text,pos) };

   if( QValidator::Acceptable == state)
    {
        m_sourceRow=row;
        count_validator->setTop(m_model->rowCount(m_sourceParent)-row+1);
        return;
    }

   if(QValidator::Intermediate==state            )
   {
       return;
   }

   ui->lineEdit_src_row->clear();
}

void MoveRowsDialog::on_count(const QString& text)
{
    auto _text{text};
    auto count{text.toInt()};

    int pos{0};
    auto state{count_validator->validate(_text,pos) };

    m_count=count;


    if(QValidator::Invalid==state)
    {
         ui->lineEdit_count->clear();
    }


 /*  if( QValidator::Acceptable == state)
    {

       m_count=count;

        return;
    }

   if(QValidator::Intermediate==state            )
   {
       return;
   }

   ui->lineEdit_count->clear();
   */
}

  std::unique_ptr<QStdItemModel::move_rows_cmd_t> MoveRowsDialog::get_cmd()
  {
   //   return cmd.release();
      return std::move(cmd);
  }

void MoveRowsDialog::on_ok()
{
            if( ui->lineEdit_count->hasAcceptableInput()
            && ui->lineEdit_dest_child->hasAcceptableInput()
            && ui->lineEdit_dest_parent_uid->hasAcceptableInput()
            && ui->lineEdit_src_row->hasAcceptableInput()
            && ui->lineEdit_src_parent_uid ->hasAcceptableInput()
            )
    {
                cmd = std::make_unique<QStdItemModel::move_rows_cmd_t>(// m_model,
                            m_sourceParent,
                                             m_sourceRow,
                                                 m_count,
                                            m_destinationParent,
                                                                       m_destinationChild

                            );
    }
            else
            {
                return;
            }


     QDialog::accept();
}

MoveRowsDialog::~MoveRowsDialog()
{
    delete ui;
}

void MoveRowsDialog::on_dest_parent(const QString& text)
{
    auto dest_uid{text.toULongLong()};

    auto dest_index{m_model->find_uid(dest_uid)};
    if(!dest_index.has_value())return;
    m_destinationParent=dest_index.value();

    dest_child_validator->setTop(m_model->rowCount(dest_index.value()));
}

void MoveRowsDialog::on_src_parent(const QString& text)
{
    auto _text{text};
     auto src_uid{text.toULongLong()};
     int pos{0};
     auto state{src_parent_validator->validate(_text,pos) };

    if( QValidator::Acceptable == state)
     {
         auto src_index{m_model->find_uid(src_uid)};
         if(!src_index.has_value())return;
        m_sourceParent=src_index.value();

         src_row_validator->setTop(m_model->rowCount(src_index.value()));
         return;
     }

    if(QValidator::Intermediate==state            )
    {
        return;
    }

    ui->lineEdit_src_parent_uid->clear();
}
