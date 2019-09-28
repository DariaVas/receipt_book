#include "helperfunctions.h"
#include "ui_add_recipe.h"
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>


void show_message_box(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void get_checked_items(const QList<QAbstractButton *> & all_items, QList<QString>& _out_selected_items)
{
    for (int i=0;  i< all_items.size(); ++i)
    {
        if (all_items[i]->isChecked())
            _out_selected_items.append(all_items[i]->text());
    }
}

bool openDataBaseConnection( QSqlDatabase& db)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("Recipe_book");
    db.setUserName("root");
    db.setPassword("1");
    if( !db.open() )
    {
        qDebug() << db.isValid();
        qDebug() << db.lastError();
        qDebug()<< "Failed to connect.";

        return false;
    }

    qDebug( "Connected to database!" );
    return true;
}

void insert_ingredients(QSqlQuery& query, QString recipe_id, QList<Ingredient> ingredients)
{
    for (int i = 0; i< ingredients.count(); ++i)
    {
        exec_query("INSERT IGNORE INTO Unit_measure SET `Unit_measure_name` = '"+ ingredients[i].measure_unit+"' ;", query);
        QString unit_measure_id = get_id_by_name(query, "Unit_measure",  ingredients[i].measure_unit);
        exec_query("INSERT IGNORE INTO Ingredient SET `Ingredient_name` = '"+ ingredients[i].name+"', "
                       "`Ingredient_cost` = '"+ QString::number(ingredients[i].cost)+"', "
                       "`Ingredient_unit_measure`="+unit_measure_id+";", query);
        QString ingredient_id = get_id_by_name(query, "Ingredient",  ingredients[i].name);
        exec_query("INSERT INTO Composition SET `Comp_Ingredient_id` = "+ingredient_id+", "
                       "`Comp_Recipe_Id`= "+recipe_id+" ,"
                       "`Quantity`= "+QString::number(ingredients[i].count)+" ;", query);
    }

}
bool add_new_recipe_to_DB(const Recipe * recipe, QSqlDatabase db)
{
    QSqlQuery query(db);

    exec_query("INSERT IGNORE INTO Cuisine SET `Cuisine_name` = '"+recipe->cuisine+"' ;", query);
    QString cuisine_id = get_id_by_name(query, "Cuisine", recipe->cuisine);

    exec_query("INSERT IGNORE INTO Basis SET `Basis_name` = '"+ recipe->base+"' ;", query);

    QString basis_id =  get_id_by_name(query, "Basis",  recipe->base);

    exec_query("INSERT INTO Recipe SET "
                   "`Rec_Cuisine_ID` = " + cuisine_id + ","
                   "`Rec_Basis_ID` = " + basis_id + ","
                   "`Description_cooking_method` = '" +  recipe->recipe_description + "',"
                   "`Recipe_name` = '" +  recipe->recipe_name + "',"
                   "`Cooking_time` = '" +  recipe->cooking_time + "',"
                   "`Servings_number` = '" +  recipe->portions_number + "',"
                   "`Caloric_content` = " +  recipe->calories + ";", query);
    QString recipe_id = get_id_by_name(query, "Recipe", "");

    QList<QString> tables_name = {"Category", "Event", "Cooking_method"};
    QList<QList<QString>> value_to_insert = { recipe->selected_categories,  recipe->selected_events, recipe->selected_methods};

    Q_ASSERT(tables_name.count()==value_to_insert.count());

    for (int i = 0; i< tables_name.count(); i++)
    {
        for(int j = 0; j < value_to_insert[i].count(); j++)
        {
            exec_query("INSERT IGNORE INTO "+tables_name[i]+" SET `"+tables_name[i]+"_name` = '"+value_to_insert[i][j]+"' ;", query);
            QString this_id = get_id_by_name(query, tables_name[i], value_to_insert[i][j]);

            exec_query("INSERT INTO Recipe_"+tables_name[i]+" SET `id_"+tables_name[i]+"` = "+this_id+", "
                           "`id_recipe` = "+recipe_id+";", query);
        }
    }

    insert_ingredients(query, recipe_id, (*recipe).ingredients);

    return true;
}

QString get_id_by_name(QSqlQuery& query, QString table_name, QString value_to_find, bool find_by_name)
{
    if (find_by_name)
    {
        value_to_find=table_name+"_name = '"+value_to_find+"'";
    }

    QString id = query.lastInsertId().toString();
    if (!id.compare(""))
    {
        if (!value_to_find.compare(""))
        {
            qFatal("Cannot get the id. And value to find is not specified!");
        }
        if(!query.exec("Select "+table_name+"_ID"+" from "+ table_name+" where "+value_to_find+" ;"))
        {
            qDebug()<<query.lastError();
            qFatal("Cannot get the id");
        }
        if (query.next())
        {
            id = query.value(0).toString();
        }
        else
        {
            qDebug()<<query.lastError();
            qFatal("Cannot get the id from query");
        }
    }
    return id;
}
void log_eror(const QSqlQuery& q)
{
    qDebug()<<q.lastError();
}
void exec_query(const QString& string_query, QSqlQuery& query)
{
  if (!query.exec(string_query))
  {
      log_eror(query);
      qDebug()<<string_query;
      throw std::runtime_error("Error");
  }
}
void update_recipe(QSqlQuery& query, QString recipe_id, QString field, QString new_value)
{
    exec_query("UPDATE `Recipe_book`.`Recipe` SET `"+field+"` = '"+new_value+"' WHERE (`Recipe_ID` = '"+recipe_id+"');", query);

}

QString ingredients_to_string(QList<Ingredient>& ingredients)
{
    QString ingred;
    while(!ingredients.isEmpty())
    {
        Ingredient ingredient = ingredients.front();
        ingredients.pop_front();
        ingred+=ingredient.name+" - "+QString::number(ingredient.count)+" "+ingredient.measure_unit+" ("+QString::number(ingredient.cost)+" грн) \n";
    }
    return ingred;
}
void select_recipes_from_db(QSqlDatabase& db, QList<Recipe> * recipes, QString recipe_filter, QString categories_filter, QString event_filter, QString cook_filter)
{
    QSqlQuery query (db);
    select_recipes(query, recipe_filter);
    read_recipes(db, recipes, query,false, categories_filter,event_filter,cook_filter);

}
void read_recipes(QSqlDatabase& db, QList<Recipe> * recipes,QSqlQuery & query, bool calculate_general_cost, QString categories_filter, QString event_filter, QString cook_filter)
{
    while (query.next())
    {
        Recipe r;
        r.recipe_id = query.value(0).toString();
        r.recipe_name = query.value(1).toString();
        r.calories = query.value(2).toString();
        r.cooking_time= query.value(3).toString();
        r.portions_number= query.value(4).toString();
        r.cuisine= query.value(5).toString();
        r.base= query.value(6).toString();
        r.recipe_description= query.value(7).toString();
        r.general_cost = calculate_general_cost ? query.value(8).toString():"";
        QSqlQuery subquery (db);

        QList<QString> tables_filter = {categories_filter, event_filter, cook_filter};
        QList<QString> tables_name = {"Category", "Event", "Cooking_method"};
        QList<QList<QString>*> lists_to_insert = { &r.selected_categories,  &r.selected_events, &r.selected_methods};


        Q_ASSERT(tables_name.count()==lists_to_insert.count() &&tables_name.count()== tables_filter.count());
        for (int i = 0; i<tables_name.count(); ++i)
        {
            select_additional_params(subquery, tables_name[i], r.recipe_id, tables_filter[i]);
            if(!subquery.size()) // Interrup
                continue;
            while (subquery.next())
            {
                lists_to_insert[i]->append(subquery.value(0).toString());
            }
        }
        exec_query(" Select Ingredient_name, unit_measure_name, Quantity, ingredient_cost "
                      "From( "
                      "Select Ingredient_name, ingredient_id, unit_measure_name, ingredient_cost "
                      "From Ingredient "
                      "Cross Join Unit_measure ON Ingredient_unit_measure=unit_measure_id) as Recipe_ingredients "
                      "Cross Join (Select Comp_Ingredient_id, Quantity From Composition Where Comp_recipe_id = "+r.recipe_id+") as Recipe_composition "
                       "ON Recipe_ingredients.ingredient_id = Recipe_composition.Comp_Ingredient_id ", subquery);
        while (subquery.next())
        {
            r.ingredients.append(Ingredient(subquery.value(0).toString(),subquery.value(1).toString(), subquery.value(2).toDouble(), subquery.value(3).toDouble()));
        }
        if (get_additional_params(db, r, categories_filter, event_filter, cook_filter, r.recipe_id))
        {
            recipes->append(r);
        }
    }
}
void select_recipes(QSqlQuery& query, QString additional_query)
{
    exec_query("SELECT Recipe_ID, Recipe_name, Caloric_content, Cooking_time, Servings_number, Cuisine_name, Basis_name, Description_cooking_method "
               "FROM Recipe "
               "CROSS JOIN Cuisine "
              "ON Cuisine.Cuisine_ID=Recipe.Rec_Cuisine_ID "
               "Cross JOIN Basis ON Basis.Basis_ID = Recipe.Rec_Basis_ID"
                + additional_query+" ; ", query);
}
void select_additional_params(QSqlQuery& query, QString table_name,QString recipe_id,  QString additional_query)
{
    exec_query("SELECT "+table_name+"_name "
                  "FROM "+table_name+" "
                  "WHERE "+table_name+"_ID = "
                  "(SELECT id_"+table_name+" "
                  "FROM Recipe_"+table_name+" "
                  "WHERE id_recipe = " + recipe_id+" "
                  "AND id_"+table_name+"="+table_name+"_ID) " +
                   additional_query + ";",query);

}
bool get_additional_params (QSqlDatabase& db, Recipe &r, QString categories_filter, QString event_filter, QString cook_filter, QString recipe_id)
{
    QList<QString> tables_filter = {categories_filter, event_filter, cook_filter};
    QList<QString> tables_name = {"Category", "Event", "Cooking_method"};
    QList<QList<QString>*> lists_to_insert = { &r.selected_categories,  &r.selected_events, &r.selected_methods};
    QSqlQuery subquery(db);

    Q_ASSERT(tables_name.count()==lists_to_insert.count() &&tables_name.count()== tables_filter.count());
    for (int i = 0; i<tables_name.count(); ++i)
    {
        select_additional_params(subquery, tables_name[i], recipe_id, tables_filter[i]);
        if(!subquery.size()) // Interrup
            return  false;
        while (subquery.next())
        {
            lists_to_insert[i]->append(subquery.value(0).toString());
        }
    }
    return true;
}
