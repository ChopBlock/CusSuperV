#ifndef APPMANGE_H
#define APPMANGE_H

#include <QObject>

class AppMange : public QObject
{
    Q_OBJECT
public:
    explicit AppMange(QObject *parent = nullptr);

signals:

};

#endif // APPMANGE_H
