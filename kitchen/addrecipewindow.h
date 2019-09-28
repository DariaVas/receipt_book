#ifndef ADDRECIPEWINDOW_H
#define ADDRECIPEWINDOW_H

#include <QMainWindow>
class QAbstractButton;
class QSqlDatabase;
class QSqlQuery;
struct Source;
struct Ingredient;
class SortWindow;
class  MainWindow;
namespace Ui {
class AddRecipeWindow;
}
struct Recipe;
class AddRecipeWindow : public QMainWindow
{
    Q_OBJECT
public:
    AddRecipeWindow( QSqlDatabase * db, QWidget *parent = nullptr);
    ~AddRecipeWindow();
    bool gather_information();
    void initialize_fields(Recipe recipe);
    void set_edit_mode(bool flag=true);
    void set_windows( SortWindow * sort_window, MainWindow * parent_window);
    void display();

private:
    Ui::AddRecipeWindow*ui;
    Recipe* recipe;
    QSqlDatabase* db;
    QString update_recipe_id;
    SortWindow * sort_window;
    MainWindow * parent_window;

public slots:
signals:
       void firstWindow();
private slots:
    void on_add_ingred_Btn_clicked();
    void on_add_recipe_Btn_clicked();
    void on_update_recipe_Btn_clicked();
    void on_show_select_recipe_clicked();
    void on_show_sort_recipe_clicked();
    void clear_fields();
};

#endif // ADDRECIPEWINDOW_H
