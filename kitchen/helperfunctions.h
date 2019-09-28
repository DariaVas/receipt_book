#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H
#include "Qt"
#include "QString"
#include "QList"
#include <QTableWidgetItem>
#include<QHeaderView>
#include <math.h>

class QAbstractButton;
class QSqlDatabase;
class QSqlQuery;
class QString;
class QMainWindow;
struct Recipe;
struct Ingredient
{
    Ingredient (QString name,QString measure_unit, double count, double cost):name(name),measure_unit(measure_unit), count(count), cost(cost)
    {}
    QString name;
    QString measure_unit;
    double count;
    double cost;
};

struct Recipe
{
    QString recipe_id;
    QString cuisine;
    QString base;
    QList<QString> selected_categories;
    QList<QString> selected_events;
    QList<QString> selected_methods;
    QString recipe_name;
    QString recipe_description;
    QString calories;
    QString portions_number;
    QString cooking_time;
    QList<Ingredient> ingredients;
    QString general_cost;
};


void get_checked_items(const QList<QAbstractButton *> & all_items, QList<QString>& _out_selected_items);
void show_message_box(QString message);
bool openDataBaseConnection(QSqlDatabase& db);
void log_eror(const QSqlQuery& q);
bool add_new_recipe_to_DB(const Recipe * recipe, QSqlDatabase db);
void insert_ingredients(QSqlQuery& query, QString recipe_id, QList<Ingredient> ingredients);
void update_recipe(QSqlQuery& query, QString recipe_id, QString field, QString new_value);
QString get_id_by_name(QSqlQuery& query, QString table_name, QString value_to_find, bool find_by_name = true);
void exec_query(const QString& string_query, QSqlQuery& query);

QString ingredients_to_string(QList<Ingredient>& ingredients);
void select_recipes_from_db(QSqlDatabase& db, QList<Recipe> * recipes, QString recipe_filter="", QString categories_filter="", QString event_filter="", QString cook_filter="");
void select_recipes(QSqlQuery& query, QString additional_query = "");
void select_additional_params(QSqlQuery& query, QString table_name,QString recipe_id,  QString additional_query = "");
bool get_additional_params (QSqlDatabase& db, Recipe &r, QString categories_filter, QString event_filter, QString cook_filter, QString recipe_id);
void read_recipes(QSqlDatabase& db, QList<Recipe> * recipes, QSqlQuery & query, bool calculate_general_cost=false,
                  QString categories_filter="", QString event_filter="", QString cook_filter="");

template<class T>
void show_recipes(T* ui, QList<Recipe>* recipes, bool show_general_cost=false){
ui->recipes_tblWidget->clear();
 ui->recipes_tblWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

if(recipes->empty())
{
    show_message_box("По вашему запросу рецептов не найдено!");
    return;
}
const int col_count = 3;
ui->recipes_tblWidget->setColumnCount(col_count);
int row_count= static_cast<int>(ceil(recipes->count()/2.0));
ui->recipes_tblWidget->setRowCount(row_count);
for (int i = 0; i<row_count; ++i)
{
    for (int j =0; j<col_count; ++j)
    {
        int index = i*col_count+j;
        if(index >=recipes->count())
            break;
        Recipe recipe = recipes->at(index);
        QTableWidgetItem * item = new QTableWidgetItem(recipe.recipe_name+"\n"
                                                       +"Время приготовления: "+ recipe.cooking_time+"\n"
                                                       +"Количество порций: "+ recipe.portions_number+"\n"
                                                       +"Количество каллорий: "+ recipe.calories+"\n"
                                                       +"Кухня: "+ recipe.cuisine+"\n"
                                                       +"Ингредиенты: "+ ingredients_to_string(recipe.ingredients)+"\n"
                                                       +"Описание: "+ recipe.recipe_description+"\n"
                                                       + (show_general_cost ? ("Общая стоимость: " + recipe.general_cost + " грн") : "")
                                                       );
        ui->recipes_tblWidget->setItem(i, j, item);
        item->setTextAlignment(Qt::AlignLeft);
    }
    ui->recipes_tblWidget->resizeRowToContents(i);
}

}

#endif // HELPERFUNCTIONS_H
