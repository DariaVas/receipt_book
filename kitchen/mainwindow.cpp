#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addrecipewindow.h"
#include "sortwindow.h"
#include "helperfunctions.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QScrollBar>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    db(new QSqlDatabase()),
    add_recipe_ui(new AddRecipeWindow(db)),
    recipes(new QList<Recipe>()),
    sort_ui(new SortWindow(db))

{
   add_recipe_ui->set_windows(sort_ui, this);
   sort_ui->set_windows(add_recipe_ui, this);
   ui->setupUi(this);
   connect(add_recipe_ui, &AddRecipeWindow::firstWindow, this, &MainWindow::show);
   ui->recipes_tblWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
   bool success = openDataBaseConnection((*db));
   if (!success)
   {
       show_message_box("Внутрення ошибка при соединении с базой данных рецепта!");
   }
   display();
}
void verticalResizeTableViewToContents(QTableView *tableView)
{
    int count=tableView->verticalHeader()->count();
    int scrollBarHeight=tableView->horizontalScrollBar()->height();
    int horizontalHeaderHeight=tableView->horizontalHeader()->height();
    int rowTotalHeight=0;
    for (int i = 0; i < count; ++i) {
        // 2018-03 edit: only account for row if it is visible
        if (!tableView->verticalHeader()->isSectionHidden(i)) {
            rowTotalHeight+=tableView->verticalHeader()->sectionSize(i);
        }
    }
    tableView->setMinimumHeight(horizontalHeaderHeight+rowTotalHeight+scrollBarHeight);
}
MainWindow::~MainWindow()
{
    delete ui;
    delete db;
    delete add_recipe_ui;
    delete recipes;
}

void MainWindow::show_add_recipe()
{
    add_recipe_ui->show();
    this->close();

 }


void MainWindow::on_show_add_recipe_page_btn_clicked()
{
    add_recipe_ui->display();
    this->hide();
}

void MainWindow::on_find_Btn_clicked()
{
    Recipe* recipe = new Recipe();
    recipe->cuisine = ui->cousine_cmBx->currentText();
    recipe->base = ui ->basis_cmBx->currentText();
    QList<QAbstractButton *> categories = ui->category_grBx->findChildren<QAbstractButton *>();

    get_checked_items(categories, recipe->selected_categories);

    QList<QAbstractButton *> events = ui->event_grBx->findChildren<QAbstractButton *>();

    get_checked_items(events, recipe->selected_events);

    QList<QAbstractButton *> methods = ui->method_grBx->findChildren<QAbstractButton *>();
    get_checked_items(methods, recipe->selected_methods);
    QString recipe_filter = "";
    bool is_previous_query=false;
    if (recipe->cuisine !="Кухня..")
    {
        is_previous_query=true;
        recipe_filter +=" WHERE  Cuisine_name='"+recipe->cuisine+"'";
    }
    if (recipe->base !="Основа..")
    {
        if(is_previous_query)
            recipe_filter+= " AND ";
        else recipe_filter+=" WHERE ";
        recipe_filter += " Basis_name= '"+recipe->base+"'";
    }
    QList<QString> tables_filter {"","",""};
    QList<QString> tables_name = {"Category", "Event", "Cooking_method"};
    QList<QList<QString>*> lists_with_data = { &recipe->selected_categories,  &recipe->selected_events, &recipe->selected_methods};
    Q_ASSERT(tables_name.count()==lists_with_data.count() &&tables_name.count()== tables_filter.count());
    for (int i =0; i< tables_name.count(); i++)
    {
        if (!lists_with_data[i]->empty())
        {
            tables_filter[i] += " AND "+tables_name[i]+"_name IN (";
            for(int j =0; j< lists_with_data[i]->size(); ++j)
            {
                tables_filter[i]+= " '"+(*lists_with_data[i])[j]+ "',";
            }
            tables_filter[i].chop(1);
            tables_filter[i]+=")";
        }
    }
    recipes->clear();
    select_recipes_from_db((*db), recipes,recipe_filter,tables_filter[0],tables_filter[1], tables_filter[2]);
    show_recipes<Ui::MainWindow>(ui, recipes);
    delete recipe;
   }

void MainWindow::on_show_sort_recipe_clicked()
{
    sort_ui->display();
    this->hide();
}
void MainWindow::display()
{
    recipes->clear();
    select_recipes_from_db(*db, recipes);
    show_recipes<Ui::MainWindow>(ui, recipes);
    this->show();
}

void MainWindow::on_recipes_tblWidget_cellDoubleClicked(int row, int column)
{
    int index = row*3+column;
    add_recipe_ui->initialize_fields(recipes->at(index));
    add_recipe_ui->set_edit_mode();
    add_recipe_ui->show();
    this->hide();
}
