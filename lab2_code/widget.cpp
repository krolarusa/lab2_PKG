#include "widget.h"
#include "ui_widget.h"

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this); //создает новую модель файловой системы
    model->setFilter(QDir::QDir::AllEntries); //устанавливает фильтр модели,
    //чтобы отображать все элементы файловой системы
    model->setRootPath(""); //устанавливает корневой путь модели как пустую строку
    ui->listView->setModel(model); //устанавливает модель файловой системы для listView,
    //чтобы отображать содержимое файловой системы в виджете списка
    ui->lineEdit->setText(" "); //устанавливает текст для lineEdit как пустую строку
    for(int i = 0; i < 5; i++)
    {
         sortcolumns[i] = 0;
    } //это цикл, который инициализирует массив sortcolumns значением 0 для каждого элемента
}

Widget::~Widget()
{
    delete ui;
}

//В этой функции обрабатывается двойной щелчок на элементе listView
void Widget::on_listView_doubleClicked(const QModelIndex &index)
{
    QListView* listView = (QListView*)sender(); //получаем указатель на listView, который вызвал сигнал
    QFileInfo fileInfo = model->fileInfo(index); //получаем информацию о файле, соответствующему индексу
    ui->lineEdit->setText(model->filePath(index)); //устанавливаем текст в lineEdit
    //как путь к выбранному файлу
    if(fileInfo.fileName() == "..")
    {
        QDir dir = fileInfo.dir();
        dir.cdUp();
        model->index(dir.absolutePath());
        listView->setRootIndex(model->index(dir.absolutePath()));
    }
    else if (fileInfo.fileName() == ".")
    {
        listView->setRootIndex(model->index(""));
    }
    else if(fileInfo.isDir())
    {
        listView->setRootIndex(index);
    }
    else if(!fileInfo.isDir())
    {
        QDir dir = fileInfo.dir();
        QString fileExt = model->fileName(index);
        QImageWriter a(model->filePath(index));
        QString resolution = "";
        QImage img (model->filePath(index));
        for(int i = fileExt.lastIndexOf('.'); i < fileExt.size(); i++)
        {
            resolution.append(fileExt[i]);
        }
        if (resolution == ".JPG" || resolution == ".gif" || resolution == ".tif" || resolution == ".bmp" ||
                resolution == ".png" || resolution == ".pcx" || resolution == ".BMP")
        {
             ui->FileName->setText(model->fileName(index));
             ui->Size->setText(QString::number(img.size().width() )+ "x" + QString::number(img.size().height() ));
             ui->Contraction->setText(QString::number(a.compression()));
             ui->Color_depth->setText(QString::number(img.bitPlaneCount()));
             ui->Resolution->setText(QString::number(img.physicalDpiX()));
        }
    }
    //проверяем является ли выбранный элемент папкой, файлом или ссылкой на родительскую директорию
    //В зависимости от условий, мы изменяем корневой индекс listView
}

//Эта функция используется для закрытия диалогового окна и показа главного виджета
void Widget::dialogClose()
{
    this->show();
}

void Widget::on_multiChoice_clicked()
{
    //создаем новое диалоговое окно table, в котором создаем таблицу twInfo
    QFileDialog *files = new QFileDialog;
    files->setWindowTitle("Open files");
    QList<QUrl> list = files->getOpenFileUrls();
    QDialog *table = new QDialog();
    table->setWindowTitle("Table");
    table->setWindowIcon(QPixmap(":/img/folder.png"));
    QGridLayout *tableLayout = new QGridLayout(table);
    table->setMinimumSize(700,700);
    twInfo = new QTableWidget(table);
    //Мы устанавливаем заголовки столбцов таблицы и их расширение
    //а также режим изменения ячеек таблицы
    tableLayout->addWidget(twInfo);
    twInfo->setColumnCount(5);
    twInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    twInfo->setRowCount(list.size());
    twInfo->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    twInfo->setHorizontalHeaderItem(1, new QTableWidgetItem("Size"));
    twInfo->setHorizontalHeaderItem(2, new QTableWidgetItem("Extension"));
    twInfo->setHorizontalHeaderItem(3, new QTableWidgetItem("Color depth"));
    twInfo->setHorizontalHeaderItem(4, new QTableWidgetItem("Contraction"));
    twInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for(int i = 0; i < list.size(); i++)
    {
        //Создаем временный объект QFile с путем к файлу из списка
        QFile temp(list[i].toLocalFile());
        //Создаем пустую строку resolution для хранения расширения файла.
        QString resolution = "";
        //Затем мы проверяем условие, что файл имеет расширение

        if (temp.fileName().lastIndexOf('.') == -1)
        {
             //уменьшаем количество строк в таблице и переходим к следующему файлу
            twInfo->setRowCount(twInfo->rowCount()-1);
            continue;
        }
        //проходим по имени файла и добавляем символы после точки в строку resolution
        for(int i = temp.fileName().lastIndexOf('.'); i < temp.fileName().size(); i++)
        {
            resolution.append(temp.fileName()[i]);
        }
        //проверяем расширение
        if (resolution != ".jpg" && resolution != ".gif" && resolution != ".tif" && resolution != ".bmp" &&
                resolution != ".png" && resolution != ".pcx" && resolution != ".BMP")
        {
            twInfo->setRowCount(twInfo->rowCount()-1);
            continue;
        }
        //заполняем соответствующие ячейки таблицы значениями, полученными из файлов или изображений
        twInfo->setItem(i, 2, new QTableWidgetItem(resolution)); //заполняем ячейку с расширением файла
        QString fileName = "";
        for(int i = temp.fileName().lastIndexOf('/') + 1; i < temp.fileName().lastIndexOf('.'); i++)
        {
            fileName.append(temp.fileName()[i]);
        }
        twInfo->setItem(i,0,new QTableWidgetItem(fileName)); // заполняем ячейку с именем файла.
        QImage im(list[i].toLocalFile());
        QImageWriter a(list[i].toLocalFile());
        //заполняем ячейку с размером файла (ширина x высота)
        twInfo->setItem(i, 1, new QTableWidgetItem(QString::number(im.size().width())+"x"+QString::number(im.size().height())));
        //заполняем ячейку с количеством плоскостей цвета
        twInfo->setItem(i, 3, new QTableWidgetItem(QString::number(im.bitPlaneCount())));
        //заполняем ячейку с коэффициентом сжатия файла
        twInfo->setItem(i, 4, new QTableWidgetItem(QString::number(a.compression())));
    }
    //если количество строк в таблице равно 0, выходим из функции
    if (twInfo->rowCount() == 0)
    {
        return;
    }
    
    //В противном случае мы отображаем диалоговое окно table, связываем сигнал закрытия окна с функцией dialogClose() и скрываем текущий виджет
    table->show();
    connect(table, SIGNAL(rejected()), this, SLOT(dialogClose()));
    connect(twInfo->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(twInfoSelected(int)));
    this->hide();
}

//Эта функция сравнивает два объекта QTableWidgetItem на основе их текстовых значений. Он возвращает true,
//если текст item1 больше текста item2, и false в противном случае.
bool column1(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text() > item2->text();
}

//Эта функция разделяет текст item1 и item2 с помощью разделителя 'x'
//сравнивает первый элемент результирующих списков строк
//Если первый элемент item1 больше первого элемента item2, он возвращает true; в противном случае он возвращает false.
bool column2(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    QStringList string1 = item1->text().split('x');
    QStringList string2 = item2->text().split('x');
    if (string1[0].toInt() > string2[0].toInt())
    {
        return true;
    }
    return false;
}

//то же, что 1
bool column3(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text() > item2 -> text();
}

//Эта функция преобразует текстовые значения item1 и item2 в целые числа с помощью
//сравнивает полученные целые числа
//Он возвращает true, если целочисленное значение item1 больше целочисленного значения item2, и false в противном случае.
bool column4(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text().toInt() > item2->text().toInt();
}

//то же, что 4
bool column5(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text().toInt() > item2->text().toInt();
}

//Этот вектор хранит указатели на вышеуказанные функции column1, column2, column3, column4 и column5
vector<function<bool(QTableWidgetItem*, QTableWidgetItem*)>> functions = {column1, column2, column3, column4, column5};

//Этот вектор инициализируется 5 элементами, для всех которых установлено значение 0
//Он отслеживает состояние сортировки для каждого столбца.
vector<int> sortcolumns(5, 0);

/*Эта функция является основной функцией, которая вызывается при выборе столбца в табличном виджете (twInfo).
 * Она выполняет сортировку на основе выбранного индекса столбца.
 * Функция сначала проверяет текущее состояние сортировки столбца.
 * Если оно равно 0 (не отсортировано), то выполняется сортировка по возрастанию с использованием соответствующей функции из вектора функций.
 * Если состояние сортировки равно 1 (по возрастанию), выполняется сортировка по убыванию.
 * Если состояние сортировки равно 2 (по убыванию), оно снова выполняет сортировку по возрастанию.
 * После сортировки состояние сортировки для выбранного столбца обновляется соответствующим образом.*/
void Widget::twInfoSelected(int columnIndex)
{
    if (sortcolumns[columnIndex] == 0)
    {
        for(int i = 0; i < twInfo->rowCount(); i++)
        {
             for(int j = 0; j < twInfo->rowCount() - 1; j++)
             {
                 if (functions[columnIndex](twInfo->item(j, columnIndex), twInfo->item(j + 1, columnIndex)))
                 {
                         for(int k = 0; k < twInfo->columnCount(); k++)
                         {
                             QString temp1 = twInfo->item(j, k)->text();
                             QString temp2 = twInfo->item(j + 1, k) -> text();
                             twInfo->setItem(j+1, k, new QTableWidgetItem(temp1));
                             twInfo->setItem(j, k, new QTableWidgetItem(temp2));
                     }
                 }
             }
         }

        for(int k = 0; k < 5; k++)
        {
            sortcolumns[k] = 0;
        }
        sortcolumns[columnIndex] = 1;
    }
    else
    {
        if (sortcolumns[columnIndex] == 1)
        {
            for(int i = 0; i < twInfo->rowCount(); i++)
            {
                 for(int j = 0; j < twInfo->rowCount() - 1; j++)
                 {
                     if (!functions[columnIndex](twInfo->item(j, columnIndex), twInfo->item(j + 1, columnIndex)))
                     {
                         for(int k = 0; k < twInfo->columnCount(); k++)
                         {
                             QString temp1 = twInfo->item(j, k)->text();
                             QString temp2 = twInfo->item(j + 1, k) -> text();
                             twInfo->setItem(j+1, k, new QTableWidgetItem(temp1));
                             twInfo->setItem(j, k, new QTableWidgetItem(temp2));
                         }
                     }
                 }
             }
            for(int k = 0; k < 5; k++)
            {
                sortcolumns[k] = 0;
            }
            sortcolumns[columnIndex] = 2;
        }
        else
        {
            for(int i = 0; i < twInfo->rowCount(); i++)
            {
                 for(int j = 0; j < twInfo->rowCount() - 1; j++)
                 {
                     if (functions[columnIndex](twInfo->item(j, columnIndex), twInfo->item(j + 1, columnIndex)))
                     {
                             for(int k = 0; k < twInfo->columnCount(); k++)
                             {
                                 QString temp1 = twInfo->item(j, k)->text();
                                 QString temp2 = twInfo->item(j + 1, k) -> text();
                                 twInfo->setItem(j+1, k, new QTableWidgetItem(temp1));
                                 twInfo->setItem(j, k, new QTableWidgetItem(temp2));
                         }
                     }
                 }
             }
            for(int k = 0; k < 5; k++)
            {
                sortcolumns[k] = 0;
            }
            sortcolumns[columnIndex] = 1;
        }
    }
}
