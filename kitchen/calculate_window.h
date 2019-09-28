/*#ifndef CALCULATE_WINDOW_H
#define CALCULATE_WINDOW_H

#include <QMainWindow>
class AddRecipeWindow;
class MainWindow;
class QSqlDatabase;
struct Recipe;
namespace Ui {
class Calculate_Window;
}

class Calculate_Window : public QDialog
{
    Q_OBJECT

public:
    Calculate_Window(QSqlDatabase* db, QWidget *parent = nullptr);
    void display();
    void set_windows( AddRecipeWindow * add_recipe, MainWindow * parent_window);

    ~Calculate_Window();

private slots:
    void on_sort_Btn_clicked();

    void on_show_select_recipe_clicked();

    void on_show_add_recipe_page_btn_clicked();

private:
    Ui::Calculate_Window *ui;
    QSqlDatabase* db;
    QList<Recipe>* recipes;
    AddRecipeWindow* add_recipe;
    MainWindow* parent_window;
};

#endif // CALCULATE_WINDOW_H*/
