#include "sortwindow.h"
#include "ui_sortwindow.h"
#include "helperfunctions.h"
#include <QSqlDatabase>
#include "QSqlQuery"
#include "addrecipewindow.h"
#include "mainwindow.h"

SortWindow::SortWindow(QSqlDatabase* db,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SortWindow),
    db(db),
    recipes(new QList<Recipe>())
{
    ui->setupUi(this);
}

void SortWindow::display()
{
    on_sort_Btn_clicked();
    this->show();
}
SortWindow::~SortWindow()
{
    delete recipes;
    delete db;
    delete ui;
}

void SortWindow::on_sort_Btn_clicked()
{
    int max_cost=ui->cost_max_spinBox->value();
    int min_cost=ui->cost_min_spinBox->value();
    int max_cal=ui->cal_max_spinBox->value();
    int min_cal=ui->cal_min_spinBox->value();
    bool timeSortFirstly = ui->time_radioButton->isChecked();
    int ascSort = ui->sort_cmbBox->currentIndex();
    QString additional_params = "WHERE Caloric_content > "+ QString::number(min_cal)+
            " AND Caloric_content < "+ QString::number(max_cal)+
            " AND recipe_cost < " + QString::number(max_cost)+
            " AND recipe_cost > " + QString::number(min_cost)+
            " ORDER BY " + (timeSortFirstly ? " cooking_time ":" Caloric_content ")+
             (ascSort ? " DESC " :  " ASC ")
            +","
            + (timeSortFirstly ? " Caloric_content ":  " cooking_time ")+
            (ascSort ? " DESC " :  " ASC ");

   QSqlQuery query (*db);
   exec_query("SELECT Recipe_ID, Recipe_name, Caloric_content, Cooking_time, Servings_number, Cuisine_name, Basis_name, Description_cooking_method, Recipe_cost \n"
              "FROM\n"
              "(\n"
              "SELECT Recipe_ID, Recipe_name, Caloric_content, Cooking_time, Servings_number, Description_cooking_method, Recipe_cost, Rec_Cuisine_ID, Rec_Basis_ID\n"
              "FROM Recipe AS r\n"
              "CROSS JOIN\n"
              "(\n"
              "SELECT SUM(ingredient_cost) AS Recipe_cost,Comp_Recipe_id\n"
              "FROM\n"
              "(SELECT Comp_Ingredient_id, Comp_Recipe_id, Ingredient_cost FROM Composition c\n"
              "CROSS JOIN\n"
              "(\n"
              " SELECT Ingredient_ID, Ingredient_cost FROM Ingredient\n"
              ") AS Recipe_composition\n"
              "ON Recipe_composition.Ingredient_ID = c.Comp_Ingredient_ID\n"
              ") AS rec_ingr GROUP BY Comp_Recipe_id\n"
              ") AS reg ON r.Recipe_ID=reg.Comp_Recipe_id\n"
              " ) AS r \n"
              "CROSS JOIN Cuisine\n"
              "ON Cuisine.Cuisine_ID=r.Rec_Cuisine_ID\n"
              "Cross JOIN Basis ON Basis.Basis_ID = r.Rec_Basis_ID "
              + additional_params + ";", query);
    recipes->clear();
    read_recipes(*db, recipes, query, true);
    show_recipes(ui, recipes, true);
}

void SortWindow::on_show_select_recipe_clicked()
{
    parent_window->display();
    this->hide();
}

void SortWindow::on_show_add_recipe_page_btn_clicked()
{
    recipes->clear();
    add_recipe->display();
    this->hide();
}
void SortWindow::set_windows( AddRecipeWindow * add_recipe, MainWindow * parent_window)
{
this->add_recipe = add_recipe;
this->parent_window = parent_window;
}

void SortWindow::on_recipes_tblWidget_cellDoubleClicked(int row, int column)
{
    int index = row*3+column;
    add_recipe->initialize_fields(recipes->at(index));
    add_recipe->set_edit_mode();
    add_recipe->show();
    this->hide();
}
