#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#include <QWidget>
#include <QStaticText>
#include <QTimer>
#include <QPainter>
#include <QLabel>

class ScrollText : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
    ScrollText(QWidget *parent = 0);

public slots:
    QString text() const;
    void setText(QString text);

    QString separator() const;
    void setSeparator(QString separator);


protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *evt);

private:
    void updateText();
    QString _text;
    QString _separator;
    QStaticText staticText;
    int singleTextWidth;
    QSize wholeTextSize;
    int leftMargin;
    bool scrollEnabled;
    int scrollPos;
    QImage alphaChannel;
    QImage buffer;
    int fontPointSize;
    QTimer timer;

private slots:
    virtual void timer_timeout();
};

#endif // SCROLLTEXT_H