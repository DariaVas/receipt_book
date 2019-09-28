#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QSqlDatabase;
namespace Ui {
class MainWindow;
}
class AddRecipeWindow;
class SortWindow;
struct Recipe;
class QTableView;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void display();

private slots:
    void show_add_recipe();


    void on_show_add_recipe_page_btn_clicked();

    void on_find_Btn_clicked();


    void on_show_sort_recipe_clicked();

    void on_recipes_tblWidget_cellDoubleClicked(int row, int column);

private:
    Ui::MainWindow*ui;
    QSqlDatabase* db;
    AddRecipeWindow* add_recipe_ui;
    QList<Recipe>* recipes;
    SortWindow* sort_ui;
};
void verticalResizeTableViewToContents(QTableView *tableView);
#endif // MAINWINDOW_H
