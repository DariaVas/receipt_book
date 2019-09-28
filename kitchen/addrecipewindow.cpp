#include "addrecipewindow.h"
#include "ui_add_recipe.h"
#include "helperfunctions.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include "sortwindow.h"
#include "mainwindow.h"

AddRecipeWindow::AddRecipeWindow( QSqlDatabase * db, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AddRecipeWindow),
    db(db)
{
    ui->setupUi(this);
    recipe = new Recipe();
    ui->update_recipe_Btn->hide();    
    ui->centralWidget->setStyleSheet("QWidget:disabled{color: rgb(58, 77, 58);}"
                                        );
}

AddRecipeWindow::~AddRecipeWindow()
{
    delete ui;
    delete recipe;
}
void set_checked_properties(QList<QCheckBox*> params, QList<QString> checked_params)
{
    const auto checkBoxList = params;
    for(auto&& singleBox : checkBoxList)
    {
        singleBox->setChecked(false);
        for(int i =0; i< checked_params.count(); ++i)
        {
            if (checked_params[i]==singleBox->text())
                singleBox->setChecked(true);
        }
    }
}
void AddRecipeWindow::set_edit_mode(bool flag)
{
    ui->cousine_cmBx->setDisabled(flag);
    ui->basis_cmBx->setDisabled(flag);
    ui->event_grBx->setDisabled(flag);
    ui->category_grBx->setDisabled(flag);
    ui->method_grBx->setDisabled(flag);
    flag ? ui->add_recipe_Btn->hide() : ui->add_recipe_Btn->show();
    flag ? ui->update_recipe_Btn->show():ui->update_recipe_Btn->hide();
}
void AddRecipeWindow::initialize_fields(Recipe recipe){
    update_recipe_id = recipe.recipe_id;
    ui->cousine_cmBx->setCurrentText(recipe.cuisine);
    ui ->basis_cmBx->setCurrentText(recipe.base);
    set_checked_properties(ui->category_grBx->findChildren<QCheckBox*>(),recipe.selected_categories);

    set_checked_properties(ui->event_grBx->findChildren<QCheckBox *>(), recipe.selected_events);
    set_checked_properties(ui->method_grBx->findChildren<QCheckBox *>(), recipe.selected_methods);
    ui->recipe_description->setPlainText(recipe.recipe_description);
    ui->recipe_name->setText(recipe.recipe_name);
    ui->calories_spinBox->setValue(recipe.calories.toInt());
    ui->portion_spinBox->setValue(recipe.portions_number.toInt());
    ui->time_spinBox->setValue(recipe.cooking_time.toInt());
    for (int i =0; i<recipe.ingredients.count(); ++i)
     {
        Ingredient ingredient = recipe.ingredients[i];
        ui->ingredientsListWidget->addItem(QString::number(ui->ingredientsListWidget->count()+1) +
                                           ". " +
                                           ingredient.name+
                                           " - "+
                                           QString::number(ingredient.count)+
                                           " "+
                                           ingredient.measure_unit +
                                           " ("+QString::number(ingredient.cost)+"грн)");
   }

}

void AddRecipeWindow::on_add_ingred_Btn_clicked()
{
    QString ingredient_name = ui->ingredient_name->text();
    double ingredient_count = ui->count_ingred_dblSpinBox->value();
    QString ingredient_measure = ui->unit_measure_cbBx->currentText();
    double ingredient_cost = ui->cost_doubleSpinBox->value();
    if(ingredient_count != double(0) && ingredient_name != "" && ingredient_measure !="")
    {
        ui->ingredientsListWidget->addItem(QString::number(ui->ingredientsListWidget->count()+1) +
                                           ". " +
                                           ingredient_name+
                                           " - "+
                                           QString::number(ingredient_count)+
                                           " "+
                                           ingredient_measure +
                                           " ("+QString::number(ingredient_cost)+"грн)");
        recipe->ingredients.append(Ingredient(ingredient_name, ingredient_measure, ingredient_count, ingredient_cost));
            }

}

bool AddRecipeWindow::gather_information()
{
    recipe->cuisine = ui->cousine_cmBx->currentText();
    recipe->base = ui ->basis_cmBx->currentText();
    QList<QAbstractButton *> categories = ui->category_grBx->findChildren<QAbstractButton *>();

    get_checked_items(categories, recipe->selected_categories);

    QList<QAbstractButton *> events = ui->event_grBx->findChildren<QAbstractButton *>();

    get_checked_items(events, recipe->selected_events);

    QList<QAbstractButton *> methods = ui->method_grBx->findChildren<QAbstractButton *>();
    get_checked_items(methods, recipe->selected_methods);
    recipe->recipe_name = ui->recipe_name->text();

    recipe->recipe_description = ui->recipe_description->toPlainText();

    recipe->calories = ui->calories_spinBox->text();
    recipe->portions_number = ui->portion_spinBox->text();
    recipe->cooking_time = ui->time_spinBox->text();
    if (recipe->selected_categories.empty() ||
            recipe->selected_events.empty() ||
            recipe->selected_methods.empty() ||
            recipe->ingredients.empty() ||
            !recipe->recipe_name.compare("") ||
            !recipe->recipe_description.compare("") ||
            !recipe->portions_number.compare("") ||
            !recipe->cooking_time.compare("")
            )
    {
        show_message_box("Заполните, пожалуйста все поля!");
        return false;
    }
    return true;
}
void AddRecipeWindow::on_add_recipe_Btn_clicked()
{

    bool result = gather_information();
    if (result)
    {
        QSqlQuery query(*db);
        try {
            exec_query("set autocommit=0; Start transaction;", query);
            add_new_recipe_to_DB(recipe, (*db));
            show_message_box("Рецепт успешно добавлен в вашу книгу!");
            clear_fields();
            exec_query("\n commit;set autocommit=1;", query);

        } catch (const std::runtime_error& error) {
            exec_query("\n rollback; set autocommit=1;", query);
            show_message_box("Ошибка при добавлении рецепта: "+QString(error.what()));
        }
    }

}

void AddRecipeWindow::on_update_recipe_Btn_clicked()
{
    QSqlQuery query((*db));
    try{
        exec_query("set autocommit=0; Start transaction;", query);
        insert_ingredients(query, update_recipe_id, recipe->ingredients);
        update_recipe(query, update_recipe_id, "Recipe_name", ui->recipe_name->text());
        update_recipe(query, update_recipe_id, "Caloric_content", ui->calories_spinBox->text());
        update_recipe(query, update_recipe_id, "Cooking_time", ui->time_spinBox->text());
        update_recipe(query, update_recipe_id, "Servings_number", ui->portion_spinBox->text());
        update_recipe(query, update_recipe_id, "Description_cooking_method", ui->recipe_description->toPlainText());
        exec_query("\n commit;set autocommit=1;", query);
        show_message_box("Рецепт успешно обновлен!");
    } catch(...)
    {
        exec_query("\n rollack;set autocommit=1;", query);
        show_message_box("Ошибка при обновлении рецепта!");
    }

}

void AddRecipeWindow::on_show_select_recipe_clicked()
{
    parent_window->display();
    this->hide();
}

void AddRecipeWindow::on_show_sort_recipe_clicked()
{
    sort_window->display();
    this->hide();
}
void AddRecipeWindow::set_windows( SortWindow * sort_window, MainWindow * parent_window)
{
   this->sort_window = sort_window;
   this->parent_window = parent_window;
}
void AddRecipeWindow::clear_fields()
{
    ui->cousine_cmBx->setCurrentIndex(0);
    ui ->basis_cmBx->setCurrentIndex(0);
    QList<QString> empty_list;
    set_checked_properties(ui->category_grBx->findChildren<QCheckBox*>(),empty_list);

    set_checked_properties(ui->event_grBx->findChildren<QCheckBox *>(), empty_list);
    set_checked_properties(ui->method_grBx->findChildren<QCheckBox *>(), empty_list);
    ui->recipe_description->setPlainText("");
    ui->recipe_name->setText("");
    ui->calories_spinBox->setValue(0);
    ui->portion_spinBox->setValue(0);
    ui->time_spinBox->setValue(0);
    ui->ingredientsListWidget->clear();
}
void AddRecipeWindow::display()
{
  set_edit_mode(false);
  clear_fields();
  this->show();
}
