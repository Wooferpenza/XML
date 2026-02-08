#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QRegularExpression>
#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("XML Парсер переменных");

    // Меню создаём в коде — так оно гарантированно отображается
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *menuFile = menuBar->addMenu(tr("Файл"));
    QAction *actionOpen = menuFile->addAction(tr("Открыть..."), this, &MainWindow::onSelectFile);
    actionOpen->setShortcut(QKeySequence::Open);
    menuFile->addAction(tr("Выход"), this, &MainWindow::onExit)->setShortcut(QKeySequence::Quit);

    QMenu *menuHelp = menuBar->addMenu(tr("Справка"));
    menuHelp->addAction(tr("О программе"), this, &MainWindow::onAbout);

    // Настройка дерева
    ui->tableVariables->setColumnCount(5);
    ui->tableVariables->setHeaderLabels(QStringList() 
        << "Имя переменной" << "Тип" << "Адрес" << "Доступ" << "Комментарий");
    ui->tableVariables->header()->setStretchLastSection(true);
    ui->tableVariables->setAlternatingRowColors(true);
    ui->tableVariables->setSortingEnabled(true);
    ui->tableVariables->setRootIsDecorated(true);

    // Информация о файле — в статус-бар (постоянно справа)
    labelStatusFile = new QLabel(tr("Файл не выбран"), this);
    statusBar()->addPermanentWidget(labelStatusFile);
    
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
        labelStatusFile->setText(tr("Файл: %1").arg(fileName));
        parseXMLFile(fileName);
    }
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("О программе"),
        tr("<h3>XML Парсер переменных</h3>"
           "<p>Просмотр переменных из XML-файлов конфигурации (Symbolconfiguration).</p>"
           "<p>Поддерживаются типы TypeSimple, TypeArray и TypeUserDef.</p>"));
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
    
    // Сначала парсим TypeList для получения маппинга типов
    typeMap.clear();
    userDefTypes.clear();
    QDomNodeList typeLists = root.elementsByTagName("TypeList");
    if (!typeLists.isEmpty()) {
        QDomElement typeList = typeLists.at(0).toElement();
        parseTypeList(typeList);
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

void MainWindow::parseTypeList(const QDomElement &typeList)
{
    QDomNode node = typeList.firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            QString tagName = element.tagName();
            
            QString typeName = element.attribute("name");
            QString iecName = element.attribute("iecname");
            
            // Обрабатываем TypeSimple, TypeArray, TypeUserDef
            if (tagName == "TypeSimple" || tagName == "TypeArray" || tagName == "TypeUserDef") {
                if (!typeName.isEmpty() && !iecName.isEmpty()) {
                    typeMap[typeName] = iecName;
                }
            }
            
            // Дополнительно обрабатываем TypeUserDef для сохранения структуры
            if (tagName == "TypeUserDef") {
                QString typeclass = element.attribute("typeclass");
                if (typeclass == "Userdef") {
                    TypeUserDefInfo userDefInfo;
                    userDefInfo.name = typeName;
                    userDefInfo.iecname = iecName;
                    userDefInfo.typeclass = typeclass;
                    
                    // Парсим UserDefElement
                    QDomNode childNode = element.firstChild();
                    while (!childNode.isNull()) {
                        if (childNode.isElement()) {
                            QDomElement childElement = childNode.toElement();
                            if (childElement.tagName() == "UserDefElement") {
                                UserDefElement elem;
                                elem.iecname = childElement.attribute("iecname");
                                elem.type = childElement.attribute("type");
                                elem.byteoffset = childElement.attribute("byteoffset");
                                userDefInfo.elements.append(elem);
                            }
                        }
                        childNode = childNode.nextSibling();
                    }
                    
                    if (!typeName.isEmpty()) {
                        userDefTypes[typeName] = userDefInfo;
                    }
                }
            }
        }
        node = node.nextSibling();
    }
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
        var.typeName = type; // Сохраняем оригинальное имя типа
        // Получаем iecname из маппинга типов, если он есть
        var.type = typeMap.contains(type) ? typeMap[type] : type;
        var.address = address;
        var.access = node.attribute("access");
        // Проверяем, является ли тип Userdef
        var.isUserDef = userDefTypes.contains(type);
        
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
    ui->tableVariables->clear();
    
    for (int i = 0; i < variables.size(); ++i) {
        const VariableInfo &var = variables.at(i);
        
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->tableVariables);
        item->setText(0, var.name);
        item->setText(1, var.type);
        item->setText(2, var.address);
        item->setText(3, var.access);
        item->setText(4, var.comment);
        
        // Если это Userdef тип, добавляем дочерние элементы
        if (var.isUserDef && userDefTypes.contains(var.typeName)) {
            const TypeUserDefInfo &userDefInfo = userDefTypes[var.typeName];
            addUserDefElements(item, var.name, var.address, userDefInfo);
            item->setExpanded(false); // По умолчанию свернуто
        }
    }
    
    // Автоматическое изменение размера столбцов
    ui->tableVariables->resizeColumnToContents(0);
    ui->tableVariables->resizeColumnToContents(1);
    ui->tableVariables->resizeColumnToContents(2);
}

void MainWindow::addUserDefElements(QTreeWidgetItem *parentItem, const QString &parentName, 
                                   const QString &parentAddress, const TypeUserDefInfo &userDefInfo)
{
    for (const UserDefElement &elem : userDefInfo.elements) {
        QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
        QString elemType = typeMap.contains(elem.type) ? typeMap[elem.type] : elem.type;
        QString elemTypeName = elem.type; // Оригинальное имя типа для проверки
        QString elemAddress = parentAddress;
        QString elemFullName = parentName + "." + elem.iecname;
        
        // Вычисляем адрес дочернего элемента (базовый адрес + смещение)
        if (!elem.byteoffset.isEmpty() && !parentAddress.isEmpty()) {
            bool ok;
            int offset = elem.byteoffset.toInt(&ok);
            if (ok) {
                QString baseAddr = parentAddress;
                // Пытаемся извлечь числовую часть адреса
                // Адреса могут быть вида: D100, M0, X0.2, Y1.3, HC202
                QRegularExpression rx("^([A-Z]+)(\\d+)(?:\\.(\\d+))?$");
                QRegularExpressionMatch match = rx.match(baseAddr);
                if (match.hasMatch()) {
                    QString prefix = match.captured(1);
                    QString numPart = match.captured(2);
                    QString bitPart = match.captured(3);
                    
                    bool numOk;
                    int baseNum = numPart.toInt(&numOk);
                    if (numOk) {
                        // Вычисляем новый адрес с учетом смещения
                        // Смещение в байтах, для D-регистров это обычно 2 байта на регистр
                        int newNum = baseNum + offset / 2; // Предполагаем, что регистры по 2 байта
                        if (bitPart.isEmpty()) {
                            elemAddress = QString("%1%2").arg(prefix).arg(newNum);
                        } else {
                            elemAddress = QString("%1%2.%3").arg(prefix).arg(newNum).arg(bitPart);
                        }
                    }
                } else {
                    // Если не удалось распарсить адрес, оставляем базовый адрес
                    // Смещение будет показано в комментарии для простых типов
                    elemAddress = parentAddress;
                }
            }
        }
        
        childItem->setText(0, elemFullName);
        childItem->setText(1, elemType);
        childItem->setText(2, elemAddress);
        childItem->setText(3, parentItem->text(3)); // Наследуем доступ от родителя
        
        // Проверяем, является ли текущий элемент UserDef типом (вложенным)
        // Для вложенных типов не показываем смещение в комментарии
        if (userDefTypes.contains(elemTypeName)) {
            // Это вложенный UserDef тип - не показываем смещение
            childItem->setText(4, "");
            const TypeUserDefInfo &nestedUserDefInfo = userDefTypes[elemTypeName];
            addUserDefElements(childItem, elemFullName, elemAddress, nestedUserDefInfo);
            childItem->setExpanded(false); // По умолчанию свернуто
        } else {
            // Это простой тип (не UserDef) - показываем смещение
            if (!elem.byteoffset.isEmpty()) {
                //childItem->setText(4, QString("Смещение: %1 байт").arg(elem.byteoffset));
            } else {
                childItem->setText(4, "");
            }
        }
    }
}
