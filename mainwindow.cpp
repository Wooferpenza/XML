#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("XML Парсер переменных");
    
    // Настройка таблицы
    ui->tableVariables->setColumnCount(5);
    ui->tableVariables->setHorizontalHeaderLabels(QStringList() 
        << "Имя переменной" << "Тип" << "Адрес" << "Доступ" << "Комментарий");
    ui->tableVariables->horizontalHeader()->setStretchLastSection(true);
    ui->tableVariables->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableVariables->setAlternatingRowColors(true);
    ui->tableVariables->setSortingEnabled(true);
    
    // Подключение сигнала кнопки
    connect(ui->btnSelectFile, &QPushButton::clicked, this, &MainWindow::onSelectFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSelectFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Выберите XML файл"), "",
        tr("XML Files (*.xml);;All Files (*)"));
    
    if (!fileName.isEmpty()) {
        ui->labelFileName->setText(QString("Файл: %1").arg(fileName));
        parseXMLFile(fileName);
    }
}

void MainWindow::parseXMLFile(const QString &fileName)
{
    variables.clear();
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", 
            QString("Не удалось открыть файл: %1").arg(fileName));
        return;
    }
    
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    
    // Используем старый API для совместимости
    if (!doc.setContent(&file, false, &errorMsg, &errorLine, &errorColumn)) {
        file.close();
        QMessageBox::warning(this, "Ошибка парсинга XML",
            QString("Ошибка в строке %1, столбце %2:\n%3")
                .arg(errorLine).arg(errorColumn).arg(errorMsg));
        return;
    }
    
    file.close();
    
    QDomElement root = doc.documentElement();
    if (root.tagName() != "Symbolconfiguration") {
        QMessageBox::warning(this, "Ошибка", 
            "Неверный формат XML файла. Ожидается корневой элемент 'Symbolconfiguration'");
        return;
    }
    
    // Поиск NodeList
    QDomNodeList nodeLists = root.elementsByTagName("NodeList");
    if (nodeLists.isEmpty()) {
        QMessageBox::information(this, "Информация", 
            "В файле не найдено NodeList");
        return;
    }
    
    QDomElement nodeList = nodeLists.at(0).toElement();
    parseNodeList(nodeList);
    
    displayVariables();
    
    statusBar()->showMessage(QString("Загружено переменных: %1").arg(variables.size()), 3000);
}

void MainWindow::parseNodeList(const QDomElement &nodeList)
{
    QDomNode node = nodeList.firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            if (element.tagName() == "Node") {
                parseNode(element);
            }
        }
        node = node.nextSibling();
    }
}

void MainWindow::parseNode(const QDomElement &node, const QString &parentPath)
{
    QString nodeName = node.attribute("name");
    if (nodeName.isEmpty()) {
        // Пропускаем узлы без имени
        QDomNode child = node.firstChild();
        while (!child.isNull()) {
            if (child.isElement()) {
                QDomElement childElement = child.toElement();
                if (childElement.tagName() == "Node") {
                    parseNode(childElement, parentPath);
                }
            }
            child = child.nextSibling();
        }
        return;
    }
    
    QString currentPath = parentPath.isEmpty() ? nodeName : parentPath + "." + nodeName;
    
    // Проверяем, является ли это переменной (имеет тип и адрес)
    QString type = node.attribute("type");
    QString address = node.attribute("directaddress");
    
    if (!type.isEmpty() && !address.isEmpty()) {
        // Это переменная
        VariableInfo var;
        var.name = currentPath;
        var.type = type;
        var.address = address;
        var.access = node.attribute("access");
        
        // Ищем комментарий
        QDomNode commentNode = node.firstChild();
        while (!commentNode.isNull()) {
            if (commentNode.isElement() && commentNode.toElement().tagName() == "Comment") {
                var.comment = commentNode.toElement().text();
                break;
            }
            commentNode = commentNode.nextSibling();
        }
        
        variables.append(var);
    }
    
    // Рекурсивно обрабатываем дочерние узлы
    QDomNode child = node.firstChild();
    while (!child.isNull()) {
        if (child.isElement()) {
            QDomElement childElement = child.toElement();
            if (childElement.tagName() == "Node") {
                parseNode(childElement, currentPath);
            }
        }
        child = child.nextSibling();
    }
}

void MainWindow::displayVariables()
{
    ui->tableVariables->setRowCount(variables.size());
    
    for (int i = 0; i < variables.size(); ++i) {
        const VariableInfo &var = variables.at(i);
        
        ui->tableVariables->setItem(i, 0, new QTableWidgetItem(var.name));
        ui->tableVariables->setItem(i, 1, new QTableWidgetItem(var.type));
        ui->tableVariables->setItem(i, 2, new QTableWidgetItem(var.address));
        ui->tableVariables->setItem(i, 3, new QTableWidgetItem(var.access));
        ui->tableVariables->setItem(i, 4, new QTableWidgetItem(var.comment));
    }
    
    // Автоматическое изменение размера столбцов
    ui->tableVariables->resizeColumnsToContents();
}
