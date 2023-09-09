#include "remote.h"
#include "ui_remote.h"
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QTimer>
#include <QUdpSocket>
#include <QDebug>
#include <QKeyEvent>
#include <QSlider>

/**************************************************************
摇杆移动处即鼠标移动事件处算法可改进
控制方向（在键盘按下事件中）算法可改进
**************************************************************/

Remote::Remote(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Remote)
{
    ui->setupUi(this);
    //初始化
    value_accel = 0;
    value_spin = 50;
    flags.rocker_move = false;
    slider_accel = new QSlider(this);
    slider_spin = new QSlider(this);
    slider_accel->setMinimum(0);
    slider_spin->setMinimum(0);
    slider_accel->setMaximum(100);
    slider_spin->setMaximum(100);
    slider_accel->setSingleStep(1);
    slider_spin->setValue(value_spin);
    slider_accel->setOrientation(Qt::Vertical);
    slider_spin->setOrientation(Qt::Horizontal);
    connect(slider_accel,&QSlider::valueChanged,[=]()
                                                {
                                                  value_accel = slider_accel->value();
                                                });
    connect(slider_spin,&QSlider::valueChanged,[=]()
                                                {
                                                  value_spin = slider_spin->value();
                                                });

    //对画出来的摇杆和滑块进行伪布局
    Rocker_Init();

    //创建定时器并连接槽函数
    rc_timer = new QTimer(this);
    rc_timer->start(10);
    connect(rc_timer,&QTimer::timeout,this,&Remote::RC_SendData);

}
//会频繁调用，因此使用内联函数，防止栈溢出
inline void Remote::Rocker_Init()
{
    value_x = 0;
    value_y = 0;
    point_inner.setX(this->width() / 4);
    point_inner.setY(this->height() - this->height() / 4);
    point_outer = point_inner;

    int len_buf = (this->width() < this->height()) ? this->width() : this->height();
    r_inner = len_buf/12;
    r_outer = r_inner*2;
    //对滑块进行伪布局
    slider_accel->setMinimumHeight(r_outer * 2);
    slider_accel->setMaximumHeight(r_outer * 2);
    slider_spin->setMinimumWidth(r_outer * 2);
    slider_spin->setMaximumWidth(r_outer * 2);
    slider_spin->move(point_inner.x() - r_outer , this->height()/4);
    slider_accel->move(this->width() - (this->width() / 8) , point_inner.y() - r_outer);

}
inline int Remote::Map(int value, int in_min, int in_max, int out_min, int out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Remote::RC_SendData()
{

    qDebug()<<value_x<<"    "<<value_y<<"    "<<value_accel<<"    "<<value_spin;
}

void Remote::paintEvent(QPaintEvent *)
{
    rc_pen.setWidth(3);
    rc_painter.begin(this);//一定要先让画家开始活动，不然画笔不会关联到画家
    rc_painter.setPen(rc_pen);
    //绘画摇杆
    rc_painter.drawEllipse(point_outer,r_outer,r_outer);
    rc_painter.drawEllipse(point_inner,r_inner,r_inner);


    rc_painter.end();

}
//键盘事件，wasd为方向，但键盘控制有缺陷，只能加满，方向暂时没有写，等待后续补全
//f减油门，space加油门，q向左旋转，e向右旋转，shift快速增加或减小，r是旋转居中，c是油门减为0
void Remote::keyPressEvent(QKeyEvent *event)
{

}

void Remote::mousePressEvent(QMouseEvent *event)
{
    //检测鼠标是否在大圆内,并更改标志位
    if(event->pos().x() < point_outer.x() + r_outer && event->pos().x() > point_outer.x() - r_outer
     &&event->pos().y() < point_outer.y() + r_outer && event->pos().y() > point_outer.y() - r_outer)
    {
        flags.rocker_move = true;
    }
    else
    {
        flags.rocker_move = false;
    }
}
//鼠标松开时，摇杆归位
void Remote::mouseReleaseEvent(QMouseEvent *)
{
    Rocker_Init();
    update();
}

void Remote::mouseMoveEvent(QMouseEvent *event)
{
    if(true == flags.rocker_move)
    {
        //由于看作矩形进行运算，算法存在一定缺陷，不影响使用，但不美观，可日后改进
        QPoint point_buf = event->pos();//暂存内圆圆心位置
        if(point_buf.x() > point_outer.x() + r_outer)
        {
            point_buf.setX(point_outer.x() + r_outer);
        }
        else if(point_buf.x() < point_outer.x() - r_outer)
        {
            point_buf.setX(point_outer.x() - r_outer);
        }
        if(point_buf.y() > point_outer.y() + r_outer)
        {
            point_buf.setY(point_outer.y() + r_outer);
        }
        else if(point_buf.y() < point_outer.y() - r_outer)
        {
            point_buf.setY(point_outer.y() - r_outer);
        }
        point_inner = point_buf;
        //计算x，y的值并映射到-100——100
        value_x = Map(point_inner.x() - point_outer.x(),-r_outer,r_outer,-100,100);
        value_y = - Map(point_inner.y() - point_outer.y(),-r_outer,r_outer,-100,100);//因为qt坐标算法问题，此处加上负号方便单片机计算
    }
    update();
}
//当窗口改变时，对摇杆进行重绘
void Remote::resizeEvent(QResizeEvent *)
{
    Rocker_Init();
}

Remote::~Remote()
{
    delete ui;
}
