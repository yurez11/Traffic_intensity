#ifndef trafic_intensity_codeH
#define trafic_intensity_codeH
//---------------------------------------------------------------------------
// Подключение необходимых заголовков
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <cstdint>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:
    // Компоненты, управляемые IDE
    TMainMenu *MainMenu;
    TMenuItem *MenuFile;
    TMenuItem *MenuFileNew;
    TMenuItem *MenuFileOpen;
    TMenuItem *MenuFileSave;
    TMenuItem *MenuFileSaveAs;
    TMenuItem *Separator1;
    TMenuItem *MenuFileExit;
    TMenuItem *MenuEdit;
    TMenuItem *MenuEditAdd;
    TMenuItem *MenuEditDelete;
    TMenuItem *MenuResults;
    TMenuItem *MenuResultsCalculate;
    TMenuItem *MenuResultsSave;
    TLabel *Label1;
	TStringGrid *StringGrid1;
    TButton *ButtonAdd;
    TButton *ButtonDelete;
    TButton *ButtonCalculate;
    TMemo *MemoResults;
    TStatusBar *StatusBar;
	TOpenDialog *OpenDialog;
    TSaveDialog *SaveDialog;
    TSaveDialog *SaveResultsDialog;
	TChart *Chart1;
    TLabel *Label2;
	TLabel *Label3;
    TMenuItem *MenuResultsSaveChart;
    TPanel *Panel1;
    TPanel *Panel2;
    TPanel *Panel3;
    TPanel *Panel4;
//    TButton *ButtonDeleteRange;
    TButton *ButtonInsert;
	TPanel *Panel5;
	TPanel *Panel6;
	TSplitter *Splitter1;
	TSplitter *Splitter2;
//    TMenuItem *ButtonEnableDrag;
//    TMenuItem *ButtonDisableDrag;

    // Обработчики событий
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall StringGrid1KeyPress(TObject *Sender, System::WideChar &Key);
    void __fastcall MenuFileNewClick(TObject *Sender);
    void __fastcall MenuFileOpenClick(TObject *Sender);
    void __fastcall MenuFileSaveClick(TObject *Sender);
    void __fastcall MenuFileSaveAsClick(TObject *Sender);
    void __fastcall MenuFileExitClick(TObject *Sender);
    void __fastcall MenuEditAddClick(TObject *Sender);
    void __fastcall MenuEditDeleteClick(TObject *Sender);
    void __fastcall MenuResultsCalculateClick(TObject *Sender);
    void __fastcall MenuResultsSaveClick(TObject *Sender);
    void __fastcall ButtonAddClick(TObject *Sender);
    void __fastcall ButtonDeleteClick(TObject *Sender);
    void __fastcall ButtonCalculateClick(TObject *Sender);
    void __fastcall MenuResultsSaveChartClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall StringGrid1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall StringGrid1DblClick(TObject *Sender);
	void __fastcall StringGrid1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall ButtonInsertClick(TObject *Sender);
    void __fastcall StringGrid1DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
	void __fastcall StringGrid1DragDrop(TObject *Sender, TObject *Source, int X, int Y);
	bool __fastcall StringGrid1SelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);

private:
    // Приватные переменные
    String CurrentFileName;      // Текущее имя файла
    bool DataModified;           // Флаг изменения данных
	bool HasDataChanges;         // Флаг наличия изменений
    void RecalculateYears();

    // Перечисление для индексов столбцов таблицы
    enum TGridColumns {
        COL_INDEX = 0,      // Номер строки
        COL_YEAR = 1,       // Год наблюдения
        COL_EMPIRICAL = 2,  // Эмпирическая интенсивность
        COL_THEORETICAL = 3,// Теоретическая интенсивность
        COL_DEVIATION = 4,  // Отклонение
        COL_ERROR = 5       // Относительная ошибка в процентах
    };

    // Структура для хранения результатов регрессии
    struct TRegressionResults
    {
        double a;               // Коэффициент a (свободный член)
        double b;               // Коэффициент b (угловой коэффициент)
        double avgN;            // Средняя интенсивность
        double stdDev;          // Среднее квадратическое отклонение
        double correlation;     // Коэффициент корреляции
        double errorPercent;    // Ошибка аппроксимации в процентах
        double forecast2000;    // Прогноз на 2000 год
    };

    // Приватные методы
    String FormatTableForExport();  // Форматирует таблицу для экспорта
    void InsertRowAtCursor();       // Вставляет строку в текущей позиции курсора
    void ShowTempStatus(const String &Message); // Показывает временный статус
    void ClearTempStatus();         // Очищает временный статус
    void SetupStatusBar();          // Настраивает строку состояния
    void DeleteSelectedRange();     // Удаляет выбранный диапазон строк
    void UpdateStatusBar();         // Обновляет строку состояния
    void ClearData();               // Очищает все данные
    void SetDataModified(bool Modified); // Устанавливает флаг изменения данных
    bool SaveData();                // Сохраняет данные в текущий файл
    bool SaveDataAs();              // Сохраняет данные в новый файл
    void LoadDataFromFile(const String &FileName);  // Загружает данные из файла
    void SaveDataToFile(const String &FileName);    // Сохраняет данные в файл
    void SetupStringGrid();         // Настраивает таблицу StringGrid
    void UpdateRowNumbers();        // Обновляет нумерацию строк
    bool CheckDataModified();       // Проверяет наличие изменений перед закрытием
    bool ValidateCell(int ACol, int ARow, String Value); // Валидирует значение ячейки
    bool ValidateAllData();         // Валидирует все данные
    bool HasValidDataForSaving();   // Проверяет наличие данных для сохранения
    TRegressionResults CalculateRegression(); // Вычисляет линейную регрессию
    void DrawRegressionChart(const TRegressionResults &results); // Рисует график регрессии
	void ShowCalculationTable(const TRegressionResults &results); // Показывает таблицу расчетов
    void TMainForm::DeleteSelectedOrCurrentRow();
public:
    // Конструктор
    __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

