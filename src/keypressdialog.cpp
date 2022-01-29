#include "keypressdialog.h"

KeyPressDialog::KeyPressDialog(QWidget *parent, QString oldKey, QString command)
    : QDialog(parent)
{
    this->setWindowTitle("Press a key");
    auto *layout = new QVBoxLayout(this);
    auto label = new QLabel("Press a key for " + command);
    auto label2 = new QLabel("Current: " + oldKey);
    layout->addWidget(label);
    layout->addWidget(label2);
    layout->setAlignment(label, Qt::AlignHCenter);
    layout->setAlignment(label2, Qt::AlignHCenter);
    this->setLayout(layout);
    this->setFixedSize(200, 150);
}

void KeyPressDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    break;
    case Qt::Key_A:
        emit keyPressed("a");
    break;
    case Qt::Key_B:
        emit keyPressed("b");
    break;
    case Qt::Key_C:
        emit keyPressed("c");
    break;
    case Qt::Key_D:
        emit keyPressed("d");
    break;
    case Qt::Key_E:
        emit keyPressed("e");
    break;
    case Qt::Key_F:
        emit keyPressed("f");
    break;
    case Qt::Key_G:
        emit keyPressed("g");
    break;
    case Qt::Key_H:
        emit keyPressed("h");
    break;
    case Qt::Key_I:
        emit keyPressed("i");
    break;
    case Qt::Key_J:
        emit keyPressed("j");
    break;
    case Qt::Key_K:
        emit keyPressed("k");
    break;
    case Qt::Key_L:
        emit keyPressed("l");
    break;
    case Qt::Key_M:
        emit keyPressed("m");
    break;
    case Qt::Key_N:
        emit keyPressed("n");
    break;
    case Qt::Key_O:
        emit keyPressed("o");
    break;
    case Qt::Key_P:
        emit keyPressed("p");
    break;
    case Qt::Key_Q:
        emit keyPressed("q");
    break;
    case Qt::Key_R:
        emit keyPressed("r");
    break;
    case Qt::Key_S:
        emit keyPressed("s");
    break;
    case Qt::Key_T:
        emit keyPressed("t");
    break;
    case Qt::Key_U:
        emit keyPressed("u");
    break;
    case Qt::Key_V:
        emit keyPressed("v");
    break;
    case Qt::Key_W:
        emit keyPressed("w");
    break;
    case Qt::Key_X:
        emit keyPressed("x");
    break;
    case Qt::Key_Y:
        emit keyPressed("y");
    break;
    case Qt::Key_Z:
        emit keyPressed("z");
    break;
    case Qt::Key_0:
        emit keyPressed("0");
    break;
    case Qt::Key_1:
        emit keyPressed("1");
    break;
    case Qt::Key_2:
        emit keyPressed("2");
    break;
    case Qt::Key_3:
        emit keyPressed("3");
    break;
    case Qt::Key_4:
        emit keyPressed("4");
    break;
    case Qt::Key_5:
        emit keyPressed("5");
    break;
    case Qt::Key_6:
        emit keyPressed("6");
    break;
    case Qt::Key_7:
        emit keyPressed("7");
    break;
    case Qt::Key_8:
        emit keyPressed("8");
    break;
    case Qt::Key_9:
        emit keyPressed("9");
    break;
    case Qt::Key_F1:
        emit keyPressed("f1");
    break;
    case Qt::Key_F2:
        emit keyPressed("f2");
    break;
    case Qt::Key_F3:
        emit keyPressed("f3");
    break;
    case Qt::Key_F4:
        emit keyPressed("f4");
    break;
    case Qt::Key_F5:
        emit keyPressed("f5");
    break;
    case Qt::Key_F6:
        emit keyPressed("f6");
    break;
    case Qt::Key_F7:
        emit keyPressed("f7");
    break;
    case Qt::Key_F8:
        emit keyPressed("f8");
    break;
    case Qt::Key_F9:
        emit keyPressed("f9");
    break;
    case Qt::Key_F10:
        emit keyPressed("f10");
    break;
    case Qt::Key_F11:
        emit keyPressed("f11");
    break;
    case Qt::Key_F12:
        emit keyPressed("f12");
    break;
    case Qt::Key_Escape:
        emit keyPressed("esc");
    break;
    case Qt::Key_Tab:
        emit keyPressed("tab");
    break;
    case Qt::Key_Backspace:
        emit keyPressed("backspace");
    break;
    case Qt::Key_Enter:
        emit keyPressed("kp_enter");
    break;
    case Qt::Key_Insert:
        emit keyPressed("kp_ins");
    break;
    case Qt::Key_Delete:
        emit keyPressed("kp_del");
    break;
    case Qt::Key_Pause:
        emit keyPressed("pause");
    break;
    case Qt::Key_SysReq:
        emit keyPressed("sysreq");
    break;
    case Qt::Key_Home:
        emit keyPressed("kp_home");
    break;
    case Qt::Key_End:
        emit keyPressed("kp_end");
    break;
    case Qt::Key_Left:
        emit keyPressed("kp_leftarrow");
    break;
    case Qt::Key_Up:
        emit keyPressed("kp_uparrow");
    break;
    case Qt::Key_Right:
        emit keyPressed("kp_rightarrow");
    break;
    case Qt::Key_Down:
        emit keyPressed("kp_downarrow");
    break;
    case Qt::Key_PageUp:
        emit keyPressed("kp_pgup");
    break;
    case Qt::Key_PageDown:
        emit keyPressed("kp_pgdn");
    break;
    case Qt::Key_Shift:
        emit keyPressed("shift");
    break;
    case Qt::Key_Control:
        emit keyPressed("ctrl");
    break;
    case Qt::Key_Alt:
        emit keyPressed("alt");
    break;
    case Qt::Key_AltGr:
        emit keyPressed("altgr");
    break;
    case Qt::Key_CapsLock:
        emit keyPressed("capslock");
    break;
    case Qt::Key_NumLock:
        emit keyPressed("numlock");
    break;
    case Qt::Key_ScrollLock:
        emit keyPressed("scrolllock");
    break;
    case Qt::Key_Space:
        emit keyPressed("space");
    break;
    case Qt::Key_Backslash:
        emit keyPressed("\\");
    break;
    }
}

void KeyPressDialog::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton:
        emit keyPressed("mouse1");
    break;
    case Qt::RightButton:
        emit keyPressed("mouse2");
    break;
    case Qt::MiddleButton:
        emit keyPressed("mouse3");
    break;
    case Qt::XButton1:
        emit keyPressed("mouse4");
    break;
    case Qt::XButton2:
        emit keyPressed("mouse5");
    break;
    default:
        break;
    }
}
