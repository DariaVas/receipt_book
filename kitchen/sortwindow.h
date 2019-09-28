#ifndef SORTWINDOW_H
#define SORTWINDOW_H

#include <QMainWindow>
class AddRecipeWindow;
class MainWindow;
class QSqlDatabase;
struct Recipe;
namespace Ui {
class SortWindow;
}

class SortWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SortWindow(QSqlDatabase* db,QWidget *parent = nullptr);
    ~SortWindow();
    void display();
    void set_windows( AddRecipeWindow * add_recipe, MainWindow * parent_window);

private slots:
    void on_sort_Btn_clicked();

    void on_show_select_recipe_clicked();

    void on_show_add_recipe_page_btn_clicked();


    void on_recipes_tblWidget_cellDoubleClicked(int row, int column);

private:
private:
    Ui::SortWindow *ui;
    QSqlDatabase* db;
    QList<Recipe>* recipes;
    AddRecipeWindow* add_recipe;
    MainWindow* parent_window;
};

#endif // SORTWINDOW_H
