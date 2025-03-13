#ifndef COMMONDEF_HPP
#define COMMONDEF_HPP

#include <QDateTime>
#include <QMap>
#include <QStringList>

#include <memory>

/************************
 * 文件只存放聲明類的代碼
*************************/

// 功能宏
// debug使用，打印出qml當前activeFocus持有者
//#define PRINT_ACTIVEFOCUS

// debug使用，打印出使用cnc_rsp通道的所有協議
//#define PRINT_CNC_RSP

// 聲明一個qml屬性的方便類宏函數
#define QML_DECL_PROPERTY(type, varName, defaVal)                                   \
Q_PROPERTY(type varName READ varName WRITE set##varName NOTIFY varName##Changed)    \
Q_SIGNALS:                                                                          \
    void varName##Changed();                                                        \
public:                                                                             \
    type get##varName(){return m_##varName;}                                        \
protected:                                                                          \
    type varName(){return m_##varName;}                                             \
    void set##varName(type value){if (m_##varName == value) return; m_##varName = value; emit varName##Changed();}    \
private:                                                                            \
    type m_##varName = defaVal;                                                     \
    type varName##_default = defaVal;

// 聲明一個qml屬性的方便類宏函數，但提供public权限的get、set方法
#define QML_DECL_PROPERTY_PUBLIC(type, varName, defaVal)                            \
Q_PROPERTY(type varName READ varName WRITE set##varName NOTIFY varName##Changed)    \
Q_SIGNALS:                                                                          \
    void varName##Changed();                                                        \
public:                                                                             \
    type varName(){return m_##varName;}                                             \
    void set##varName(type value){if (m_##varName == value) return; m_##varName = value; emit varName##Changed();}    \
private:                                                                            \
    type m_##varName = defaVal;                                                     \
    type varName##_default = defaVal;

#endif // COMMONDEF_HPP
