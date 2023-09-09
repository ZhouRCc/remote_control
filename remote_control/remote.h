#ifndef REMOTE_H
#define REMOTE_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QUdpSocket>
#include <QTimer>
#include <QSlider>

namespace Ui {
class Remote;
}

class Remote : public QWidget
{
    Q_OBJECT

public:
    explicit Remote(QWidget *parent = nullptr);
    void Rocker_Init();//摇杆初始化伪布局函数，减少代码重复
    //映射函数，将value从in_max到in_min区间映射到out_max到out_min区间
    int Map(int value,int in_min,int in_max,int out_min,int out_max);
    ~Remote();
public slots:
    void RC_SendData();//发送数据

signals:

private:
    Ui::Remote *ui;
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QPainter rc_painter;
    QPen rc_pen;
    int r_outer;//外圆半径
    int r_inner;//内圆半径
    QPoint point_outer;//外圆圆心
    QPoint point_inner;//内圆圆心
    QTimer* rc_timer;//定时器
    QSlider* slider_accel;//油门滑块
    QSlider* slider_spin;//旋转滑块
    //x,y,油门，旋转值
    int value_x;
    int value_y;
    int value_accel;
    int value_spin;

    struct FLAGS
    {
        unsigned rocker_move : 1;//摇杆是否移动标志位
    }flags;//集中存放标志位

};

#endif // REMOTE_H
