#ifndef SORT_WINDOW_H
#define SORT_WINDOW_H

#include <QWidget>
class AddRecipeWindow;
class MainWindow;
namespace Ui {
class Sort_Window;
}
class QSqlDatabase;
struct Recipe;
class Sort_Window : public QWidget
{
    Q_OBJECT

public:
    Sort_Window(QSqlDatabase* db, QWidget *parent = nullptr);
    void display();
    void set_windows( AddRecipeWindow * add_recipe, MainWindow * parent_window);

    ~Sort_Window();

private slots:
    void on_sort_Btn_clicked();

    void on_show_select_recipe_clicked();

    void on_show_add_recipe_page_btn_clicked();

private:
    Ui::Sort_Window *ui;
    QSqlDatabase* db;
    QList<Recipe>* recipes;
    AddRecipeWindow* add_recipe;
    MainWindow* parent_window;
};


#endif // SORT_WINDOW_H
