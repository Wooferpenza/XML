#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct VariableInfo {
    QString name;
    QString type;
    QString address;
    QString access;
    QString comment;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectFile();
    
private:
    void parseXMLFile(const QString &fileName);
    void parseTypeList(const QDomElement &typeList);
    void parseNodeList(const QDomElement &nodeList);
    void parseNode(const QDomElement &node, const QString &parentPath = "");
    void displayVariables();
    
    Ui::MainWindow *ui;
    QList<VariableInfo> variables;
    QMap<QString, QString> typeMap; // Маппинг имени типа на iecname
};

#endif // MAINWINDOW_H
