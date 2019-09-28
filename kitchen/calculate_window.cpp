/*#include "calculate_window.h"
#include "ui_calculate_window.h"
#include "helperfunctions.h"
#include <QSqlDatabase>
#include "QSqlQuery"
#include "addrecipewindow.h"
#include "mainwindow.h"

Calculate_Window::Calculate_Window(QSqlDatabase* db, QWidget *parent):
    QDialog(parent),
    ui(new Ui::Calculate_Window),
    db(db),
    recipes(new QList<Recipe>())
{
    ui->setupUi(this);
}
void Calculate_Window::display()
{
    select_recipes_from_db(*db, recipes);
    show_recipes<Ui::Calculate_Window>(ui, recipes);
}
Calculate_Window::~Calculate_Window()
{
    delete recipes;
    delete db;
    delete ui;
}

void Calculate_Window::on_sort_Btn_clicked()
{
    int max_cost=ui->cost_max_spinBox->value();
    int min_cost=ui->cost_min_spinBox->value();
    int max_cal=ui->cal_max_spinBox->value();
    int min_cal=ui->cal_min_spinBox->value();
    bool timeSortFirstly = ui->time_radioButton->isChecked();
    bool ascSort = ui->sort_cmbBox->currentIndex();
    QString additional_params = "WHERE Caloric_content > "+ QString::number(min_cal)+
            " AND Caloric_content < "+ QString::number(max_cal)+
            " AND recipe_cost < " + QString::number(max_cost)+
            " AND recipe_cost > " + QString::number(min_cost)+
            " ORDER BY " + (timeSortFirstly ? "recipe_cost, Caloric_content": "Caloric_content,recipe_cost")+
             (ascSort ? " ASC" : " Desc");

   QSqlQuery query (*db);
   exec_query("SELECT Recipe_ID, Recipe_name, Caloric_content, Cooking_time, Servings_number, Cuisine_name, Basis_name, Description_cooking_method, Recipe_cost"
        " From Recipe r"
        "CROSS JOIN "
        "( "
        "SELECT SUM(ingredient_cost) AS Recipe_cost,Comp_Recipe_id "
        "FROM "
        "(SELECT Comp_Ingredient_id, Comp_Recipe_id, Ingredient_cost FROM Composition c "
        "CROSS JOIN "
        "( "
         "SELECT Ingredient_ID, Ingredient_cost FROM Ingredient "
        ") AS Recipe_composition "
        "ON Recipe_composition.Ingredient_ID = c.Comp_Ingredient_ID "
        ") AS rec_ingr GROUP BY Comp_Recipe_id "
        ") AS reg ON r.Recipe_ID=reg.Comp_Recipe_id " + additional_params, query );
    QList<Recipe> * recipes = new QList<Recipe>();
    read_recipes(*db, recipes, query);
    show_recipes(ui, recipes);
    delete recipes;
}

size_t find_recipe_cost(QString recipe_id, QSqlQuery& query)
{
    exec_query("SELECT SUM(ingredient_cost) FROM ( SELECT ingredient_cost"
         "FROM("
         "SELECT ingredient_id, ingredient_cost"
          "FROM Ingredient"
          "CROSS JOIN (SELECT Comp_Ingredient_id FROM Composition WHERE Comp_recipe_id = "+recipe_id+") AS Recipe_composition"
          "ON Ingredient.ingredient_id = Recipe_composition.Comp_Ingredient_id) AS rec_ingr) AS ingred;", query);
    while (query.next()) {
        return query.value(0).toUInt();
    }
    return 0;
}

void Calculate_Window::on_show_select_recipe_clicked()
{
    parent_window->show();
    this->hide();
}

void Calculate_Window::on_show_add_recipe_page_btn_clicked()
{
    add_recipe->show();
    this->hide();
}
void Calculate_Window::set_windows( AddRecipeWindow * add_recipe, MainWindow * parent_window)
{
this->add_recipe = add_recipe;
this->parent_window = parent_window;
}
*/
