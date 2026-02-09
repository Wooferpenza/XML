#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>
#include <QList>
#include <Qt>

QT_BEGIN_NAMESPACE
class QLabel;
class QTreeWidgetItem;
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct UserDefElement {
    QString iecname;
    QString type;
    QString byteoffset;
};

struct TypeUserDefInfo {
    QString name;
    QString iecname;
    QString typeclass;
    QList<UserDefElement> elements;
};

struct VariableInfo {
    QString name;
    QString type;
    QString typeName; // Оригинальное имя типа (T_XXX)
    QString address;
    QString access;
    QString comment;
    bool isUserDef;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectFile();
    void onExportFile();
    void onExit();
    void onAbout();
    void onVariableItemChanged(QTreeWidgetItem *item, int column);

private:
    void parseXMLFile(const QString &fileName);
    void parseTypeList(const QDomElement &typeList);
    void parseNodeList(const QDomElement &nodeList);
    void parseNode(const QDomElement &node, const QString &parentPath = "");
    void displayVariables();
    void addUserDefElements(QTreeWidgetItem *parentItem, const QString &parentName, 
                           const QString &parentAddress, const TypeUserDefInfo &userDefInfo);
    void setChildrenCheckState(QTreeWidgetItem *parent, Qt::CheckState state);

    Ui::MainWindow *ui;
    QLabel *labelStatusFile;  // путь к файлу в статус-баре
    QList<VariableInfo> variables;
    QMap<QString, QString> typeMap; // Маппинг имени типа на iecname
    QMap<QString, TypeUserDefInfo> userDefTypes; // Информация о пользовательских типах
};

#endif // MAINWINDOW_H
