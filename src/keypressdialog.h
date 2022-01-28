#ifndef KEYPRESSDIALOG_H
#define KEYPRESSDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QVBoxLayout>

class KeyPressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KeyPressDialog(QWidget *parent = nullptr,
                            QString oldKey = nullptr,
                            QString command = nullptr);
private:
    void keyPressEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

signals:
    void keyPressed(QString keyName);
};

#endif // KEYPRESSDIALOG_H
