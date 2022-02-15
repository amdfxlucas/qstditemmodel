#include "myqtreeview.h"


#include <QGraphicsOpacityEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainter>



// namespace{

// USAGE:
 // QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
// blur->setBlurRadius(8);

// QImage source(":/image/altro_debolon.png");
// QImage result = applyEffectToImage(source, blur);
// result.save("final.png");




/*

QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent = 0)
{
    if(src.isNull()) return QImage();   //No need to do anything else!
    if(!effect) return src;             //No need to do anything else!
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
  //  res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
    return res;
}

QImage applyEffectToImage(QPixmap src, QGraphicsEffect *effect, int extent = 0)
{
    if(src.isNull()) return QImage();   //No need to do anything else!
    // if(!effect) return src;             //No need to do anything else!

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(src);
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
    return res;
}


void test()
{
    QImage bkgnd(":/image/altro_debolon.png");
  //    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

        auto opacity_effect{ new QGraphicsOpacityEffect()};
          opacity_effect->setOpacity(0.5);

          auto backgr{applyEffectToImage(bkgnd,opacity_effect)};
        backgr.save("opac_img05.png");
}

}
*/

void MyQTreeView::paintEvent(QPaintEvent *event)
{







            QTreeView::paintEvent(event);

}

MyQTreeView::MyQTreeView(QWidget* parent )
    : QTreeView(parent)
{

 //  test();

}


/*
 //     QPixmap bkgnd(":/image/altro_debolon.png");
        QImage bkgnd(":/image/altro_debolon.png");
      //    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

            auto opacity_effect{ new QGraphicsOpacityEffect(this)};
              opacity_effect->setOpacity(0.5);

              auto backgr{applyEffectToImage(bkgnd,opacity_effect)};
            backgr.save("opac_img.png");

          QPalette palette=this->palette();
          palette.setBrush(QPalette::Window, backgr);
          this->setPalette(palette);

          setMask(bkgnd.mask());
 *
 */
