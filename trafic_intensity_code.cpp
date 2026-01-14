#include <vcl.h>
#include <System.IOUtils.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <PngImage.hpp>
#include <jpeg.hpp>
#pragma hdrstop
#include "trafic_intensity_code.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

// Конструктор класса
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

// Инициализация формы при создании
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	// Настройка компонентов
	Chart1->Title->Text->Clear();
	OpenDialog->Filter = "Данные интенсивности движения (*.t_i_k)|*.t_i_k|Все файлы (*.*)|*.*";
	SaveDialog->Filter = "Данные интенсивности движения(*.t_i_k)|*.t_i_k|Все файлы (*.*)|*.*";
	SaveDialog->Options << ofOverwritePrompt;
    SaveResultsDialog->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
    OpenDialog->Title = "Интенсивность движения";
	SaveDialog->Title = "Интенсивность движения";
    SaveResultsDialog->Title = "Интенсивность движения";
    SaveResultsDialog->DefaultExt = "txt";
	SaveDialog->DefaultExt = "t_i_k";


	// Инициализация переменных
	HasDataChanges = false;
    CurrentFileName = "";
    if (MenuFileSave) {
        MenuFileSave->Enabled = false;
    }
    DataModified = false;

    // Настройка интерфейса
    SetupStatusBar();
    SetupStringGrid();

    // Настройка горячих клавиш
    if (MenuFileNew)       MenuFileNew->ShortCut = ShortCut(L'N', TShiftState() << ssCtrl);
    if (MenuFileOpen)      MenuFileOpen->ShortCut = ShortCut(L'O', TShiftState() << ssCtrl);
    if (MenuFileSave)      MenuFileSave->ShortCut = ShortCut(L'S', TShiftState() << ssCtrl);
    if (MenuFileSaveAs)    MenuFileSaveAs->ShortCut = ShortCut(L'S', TShiftState() << ssCtrl << ssShift);
    if (MenuFileExit)      MenuFileExit->ShortCut = ShortCut(VK_F4, TShiftState() << ssAlt);

	if (MenuEditAdd)       MenuEditAdd->ShortCut = ShortCut(L'I', TShiftState() << ssCtrl);
	if (MenuEditDelete)    MenuEditDelete->ShortCut = ShortCut(L'D', TShiftState()<< ssCtrl);

    if (MenuResultsCalculate) MenuResultsCalculate->ShortCut = ShortCut(VK_F5, TShiftState());
    if (MenuResultsSave)      MenuResultsSave->ShortCut = ShortCut(L'R', TShiftState() << ssCtrl);
    if (MenuResultsSaveChart) MenuResultsSaveChart->ShortCut = ShortCut(L'G', TShiftState() << ssCtrl);

    // Инициализация таблицы
    StringGrid1->RowCount = 1;
    UpdateRowNumbers();
	UpdateStatusBar();
}
//---------------------------------------------------------------------------

//Обновление строки состояния
void TMainForm::UpdateStatusBar()
{
    String status;
    if (CurrentFileName.IsEmpty()) {
        status = "Безымянный";
    } else {
        status = ExtractFileName(CurrentFileName);
    }

    if (DataModified) {
        status = status + " *";
    }

    if (StatusBar->Panels->Count > 0) {
        StatusBar->Panels->Items[0]->Text = status;
    } else {
        StatusBar->SimpleText = status;
    }
}

//Настройка строки состояния
void TMainForm::SetupStatusBar()
{
    if (StatusBar->Panels->Count == 0) {
        StatusBar->Panels->Add();
        StatusBar->Panels->Add();
    }

    StatusBar->Panels->Items[0]->Width = 200;
    StatusBar->Panels->Items[1]->Width = 300;
}

//Показывает временное сообщение в строке состояния
void TMainForm::ShowTempStatus(const String &Message)
{
	if (StatusBar->Panels->Count > 1) {
		StatusBar->Panels->Items[1]->Text = Message;
	} else {
		StatusBar->SimpleText = Message;
	}
}

//Очистка временного сообщения
void TMainForm::ClearTempStatus()
{
	if (StatusBar->Panels->Count > 1) {
		StatusBar->Panels->Items[1]->Text = "";
	}
}

//Настраивает таблицу StringGrid
void TMainForm::SetupStringGrid()
{
	StringGrid1->FixedCols = 1;
	StringGrid1->FixedRows = 1;
	StringGrid1->ColCount = 6;

	// Устанавливаем заголовки столбцов
	StringGrid1->Cells[COL_INDEX][0] = "№";
	StringGrid1->Cells[COL_YEAR][0] = "Год";
	StringGrid1->Cells[COL_EMPIRICAL][0] = "Интенсивность, авт/сут.";
	StringGrid1->Cells[COL_THEORETICAL][0] = "Теоретическая N";
	StringGrid1->Cells[COL_DEVIATION][0] = "Отклонение";
	StringGrid1->Cells[COL_ERROR][0] = "Отн. ошибка %";
}




//Обновляет нумерацию строк и ширину столбцов
void TMainForm::UpdateRowNumbers()
{
	//Обновление номера строк
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		StringGrid1->Cells[COL_INDEX][i] = IntToStr(i);
	}

	//Автоматически подбирает ширину столбцов
	TCanvas* canvas = StringGrid1->Canvas;
    for (int col = 0; col < StringGrid1->ColCount; col++) {
        int maxWidth = 0;
        String headerText = StringGrid1->Cells[col][0];
        int width = canvas->TextWidth(headerText) + 20;
        if (width > maxWidth) maxWidth = width;

        for (int row = 1; row < StringGrid1->RowCount; row++) {
            String cellText = StringGrid1->Cells[col][row];
            if (!cellText.IsEmpty()) {
                width = canvas->TextWidth(cellText) + 8;
                if (width > maxWidth) maxWidth = width;
            }
        }

        if (maxWidth < 30) maxWidth = 30;
        StringGrid1->ColWidths[col] = maxWidth;
	}
}

//Очищает все данные в таблице
void TMainForm::ClearData()
{
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        StringGrid1->Cells[COL_YEAR][i] = "";
        StringGrid1->Cells[COL_EMPIRICAL][i] = "";
		StringGrid1->Cells[COL_THEORETICAL][i] = "";
        StringGrid1->Cells[COL_DEVIATION][i] = "";
        StringGrid1->Cells[COL_ERROR][i] = "";
    }

	StringGrid1->RowCount = 1;
    StringGrid1->Cells[COL_YEAR][1] = "";
    StringGrid1->Cells[COL_EMPIRICAL][1] = "";
    MemoResults->Lines->Clear();
    SetDataModified(false);
    DataModified = false;
    HasDataChanges = false;

    if (MenuFileSave) {
        MenuFileSave->Enabled = false;
    }

    UpdateStatusBar();
    UpdateRowNumbers();
    StringGrid1->Invalidate();
}

//Устанавливает флаг изменения данных
void TMainForm::SetDataModified(bool Modified)
{
    DataModified = Modified;
    HasDataChanges = true;

    if (MenuFileSave) {
		MenuFileSave->Enabled = DataModified;
    }

    UpdateStatusBar();
}

//Проверяет наличие изменений и предлагает сохранить
bool TMainForm::CheckDataModified()
{
    if (DataModified) {
        if (!HasValidDataForSaving()) {
            SetDataModified(false);
            return true;
        }

        String message;
        if (CurrentFileName.IsEmpty()) {
            message = L"Сохранить изменения в данных?";
        } else {
            String fileName = ExtractFileName(CurrentFileName);
            message = L"Сохранить изменения в файле '" + fileName + L"'?";
        }

        int result = Application->MessageBox(
            message.w_str(),
            L"Интенсивность движения",
            MB_YESNOCANCEL | MB_ICONQUESTION);

        if (result == IDYES) {
            return SaveData();
        } else if (result == IDNO) {
            return true;
        } else if (result == IDCANCEL) {
            return false;
        }
    }
    return true;
}

//Сохраняет данные в текущий файл
bool TMainForm::SaveData()
{
    if (!HasValidDataForSaving()) {
        String message;
        if (CurrentFileName.IsEmpty()) {
            message = L"Нет данных для сохранения. Добавьте хотя бы одну строку с годом и интенсивностью.";
        } else {
            String fileName = ExtractFileName(CurrentFileName);
            message = L"Нет данных для сохранения в файл '" + fileName +
                      L"'. Добавьте хотя бы одну строку с годом и интенсивностью.";
        }

        Application->MessageBox(
            message.w_str(),
            L"Интенсивность движения",
            MB_OK | MB_ICONWARNING
		);
		return false;
	}

	if (CurrentFileName.IsEmpty()) {
		return SaveDataAs();
	} else {
		SaveDataToFile(CurrentFileName);
		return true;
	}
}

//Сохраняет данные в новый файл
bool TMainForm::SaveDataAs()
{
    if (!HasValidDataForSaving()) {
        String message;
        if (CurrentFileName.IsEmpty()) {
            message = L"Нет данных для сохранения. Добавьте хотя бы одну строку с годом и интенсивностью.";
        } else {
            String fileName = ExtractFileName(CurrentFileName);
            message = L"Нет данных для сохранения из файла '" + fileName +
                      L"'. Добавьте хотя бы одну строку с годом и интенсивностью.";
        }
        Application->MessageBox(
            message.w_str(),
            L"Интенсивность движения",
            MB_OK | MB_ICONWARNING
        );
        return false;
    }

    SaveDialog->FileName = CurrentFileName.IsEmpty() ? "" : CurrentFileName;

    if (SaveDialog->Execute()) {
        String newFileName = SaveDialog->FileName;
        SaveDataToFile(newFileName);
        return true;
    }

    return false;
}

//Проверяет наличие валидных данных для сохранения
bool TMainForm::HasValidDataForSaving()
{
	for (int i = 1; i < StringGrid1->RowCount; i++) {
		String yearStr = StringGrid1->Cells[COL_YEAR][i];
        String intensityStr = StringGrid1->Cells[COL_EMPIRICAL][i];

        if (!yearStr.IsEmpty() && !intensityStr.IsEmpty()) {
            return true;
        }
    }
    return false;
}

//Загружает данные из файла
void TMainForm::LoadDataFromFile(const String &FileName)
{
    TFileStream *fs = NULL;
    try {
        fs = new TFileStream(FileName, fmOpenRead);

        int recordCount;
        fs->Read(&recordCount, sizeof(recordCount));

        if (recordCount < 1 || recordCount > 1000) {
            ShowTempStatus("Ошибка: некорректное количество записей в файле");
            throw Exception("Некорректное количество записей в файле");
        }

        StringGrid1->RowCount = recordCount + 1;

        for (int i = 0; i < recordCount; i++) {
            int16_t year;
            uint32_t intensityFixed;
            fs->Read(&year, sizeof(year));
            fs->Read(&intensityFixed, sizeof(intensityFixed));
            double intensityVal = static_cast<double>(intensityFixed) / 10.0;
            StringGrid1->Cells[COL_YEAR][i+1] = IntToStr(year);

            if (intensityVal == floor(intensityVal)) {
                StringGrid1->Cells[COL_EMPIRICAL][i+1] = IntToStr(static_cast<int>(intensityVal));
            } else {
                StringGrid1->Cells[COL_EMPIRICAL][i+1] = FloatToStrF(intensityVal, ffFixed, 12, 1);
            }
        }

        CurrentFileName = FileName;
        SetDataModified(false);
        HasDataChanges = false;
        if (MenuFileSave) {
            MenuFileSave->Enabled = HasDataChanges;
        }

        UpdateStatusBar();
        UpdateRowNumbers();
		ShowTempStatus("Данные успешно загружены из файла: " + ExtractFileName(FileName));
    }
	catch (Exception &e) {
		String errorMsg = "Ошибка загрузки файла: " + e.Message;
        ShowTempStatus(errorMsg);

        String errorMessage = "Ошибка загрузки файла '" + FileName + "':\n" + e.Message;
        Application->MessageBox(
            errorMessage.w_str(),
            L"Интенсивность движения",
            MB_OK | MB_ICONERROR
        );
        if (fs) delete fs;
        throw;
    }
    if (fs) delete fs;
}

//Валидирует все данные в таблице
bool TMainForm::ValidateAllData()
{
    TStringList* errors = new TStringList();
    try {
        for (int i = 1; i < StringGrid1->RowCount; i++) {
            String yearStr = StringGrid1->Cells[COL_YEAR][i];
            String intensityStr = StringGrid1->Cells[COL_EMPIRICAL][i];

            if (yearStr.IsEmpty() && intensityStr.IsEmpty()) {
                continue;
            }

			//Валидация года
            if (yearStr.IsEmpty()) {
                errors->Add("Строка " + IntToStr(i) + ": Год не может быть пустым");
            } else {
                bool yearInvalidCharFound = false;
                for (int pos = 1; pos <= yearStr.Length(); ++pos) {
                    wchar_t ch = yearStr[pos];
                    if (ch < '0' || ch > '9') {
                        String msg = "Строка " + IntToStr(i) + ": Некорректный символ '" + String(ch) +
                                     "' на позиции " + IntToStr(pos) +
                                     " в строке: \"" + yearStr + "\"";
                        errors->Add(msg);
                        yearInvalidCharFound = true;
                        break;
                    }
                }
                if (!yearInvalidCharFound) {
                    try {
                        int year = StrToInt(yearStr);
                        if (year < 1900 || year > 2100) {
                            errors->Add("Строка " + IntToStr(i) + ": Год должен быть в диапазоне 1900–2100 (указано: \"" + yearStr + "\")");
                        }
                    } catch (EConvertError &) {
                        errors->Add("Строка " + IntToStr(i) + ": Некорректный формат года: \"" + yearStr + "\"");
                    }
                }
            }

			//Валидация интенсивности
            if (intensityStr.IsEmpty()) {
                errors->Add("Строка " + IntToStr(i) + ": Интенсивность не может быть пустой");
            } else {
                bool intensityInvalidCharFound = false;
                for (int pos = 1; pos <= intensityStr.Length(); ++pos) {
                    wchar_t ch = intensityStr[pos];
                    if ((ch >= '0' && ch <= '9') || ch == '.' || ch == ',' || ch == '-') {
                        if (ch == '-' && pos != 1) {
                            String msg = "Строка " + IntToStr(i) + ": Некорректный символ '-' на позиции " +
                                     IntToStr(pos);
                            errors->Add(msg);
                            intensityInvalidCharFound = true;
                            break;
                        }
                    } else {
                        String msg = "Строка " + IntToStr(i) + ": Некорректный символ '" + String(ch) +
                                     "' на позиции " + IntToStr(pos) +
                                     " в строке: \"" + intensityStr + "\"";
                        errors->Add(msg);
                        intensityInvalidCharFound = true;
                        break;
                    }
                }
                if (!intensityInvalidCharFound) {
                    String cleanIntensity = StringReplace(intensityStr, ",", ".", TReplaceFlags() << rfReplaceAll);
                    try {
                        double intensity = StrToFloat(cleanIntensity);
                        if (intensity < 0) {
                            errors->Add("Строка " + IntToStr(i) + ": Интенсивность не может быть отрицательной (указано: \"" + intensityStr + "\")");
                        }
                        if (intensity > 429496729.5) {
                            errors->Add("Строка " + IntToStr(i) + ": Интенсивность превышает максимальное значение 429 млн (указано: \"" + intensityStr + "\")");
                        }
                    } catch (EConvertError &) {
                        errors->Add("Строка " + IntToStr(i) + ": Некорректный формат интенсивности: \"" + intensityStr + "\"");
                    }
                }
            }
        }

        if (errors->Count > 0) {
            String fullMessage = "Обнаружены ошибки в данных:\n";
            for (int j = 0; j < errors->Count; j++) {
                fullMessage += "- " + errors->Strings[j] + "\n";
            }
            ShowTempStatus("Обнаружены ошибки в данных");
            throw Exception(fullMessage);
        }

        ShowTempStatus("Данные корректны");
        return true;
    }
    __finally {
        delete errors;
    }
}

//Сохраняет данные в файл
void TMainForm::SaveDataToFile(const String &FileName)
{
    if (!HasValidDataForSaving()) {
        String msg = "Нет данных для сохранения. Добавьте хотя бы одну строку с годом и интенсивностью.";
        ShowTempStatus(msg);
        throw Exception(msg);
    }

    TFileStream *fs = NULL;
    try {
        fs = new TFileStream(FileName, fmCreate);

        int recordCount = 0;
        for (int i = 1; i < StringGrid1->RowCount; i++) {
            if (!StringGrid1->Cells[COL_YEAR][i].IsEmpty() && !StringGrid1->Cells[COL_EMPIRICAL][i].IsEmpty()) {
                recordCount++;
            }
        }

        fs->Write(&recordCount, sizeof(recordCount));

        for (int i = 1; i < StringGrid1->RowCount; i++) {
            if (StringGrid1->Cells[COL_YEAR][i].IsEmpty() && StringGrid1->Cells[COL_EMPIRICAL][i].IsEmpty()) {
                continue;
            }

            int16_t year = StrToIntDef(StringGrid1->Cells[COL_YEAR][i], 0);
            String intensityStr = StringReplace(StringGrid1->Cells[COL_EMPIRICAL][i], ",", ".", TReplaceFlags() << rfReplaceAll);
            double intensityVal = StrToFloat(intensityStr);
            uint32_t intensityFixed = static_cast<uint32_t>(intensityVal * 10.0 + 0.5);
            fs->Write(&year, sizeof(year));
            fs->Write(&intensityFixed, sizeof(intensityFixed));
        }

        CurrentFileName = FileName;
        SetDataModified(false);
        HasDataChanges = false;
        String successMessage = "Данные успешно сохранены в файл:\n" + FileName;
        ShowTempStatus("Данные успешно сохранены в файл: " + ExtractFileName(FileName));

//        Application->MessageBox(
//			successMessage.w_str(),
//			L"Интенсивность движения",
//			MB_OK | MB_ICONINFORMATION
//		);

        if (MenuFileSave) {
            MenuFileSave->Enabled = HasDataChanges;
        }

        ShowTempStatus("Данные успешно сохранены");
    }
    catch (Exception &e) {
        String errorMessage = "Ошибка сохранения файла '" + FileName + "':\n" + e.Message;
        ShowTempStatus(errorMessage);

        Application->MessageBox(
            errorMessage.w_str(),
            L"Ошибка",
            MB_OK | MB_ICONERROR
        );
        if (fs) {
            delete fs;
            if (FileExists(FileName)) {
                DeleteFile(FileName);
            }
        }
        throw;
    }
    if (fs) delete fs;
}

//Валидирует значение ячейки
bool TMainForm::ValidateCell(int ACol, int ARow, String Value)
{
    if (ARow == 0) return true;
    if (ACol == 0) return true;

    if (ACol == COL_YEAR) {
        if (Value.IsEmpty()) {
            ShowTempStatus("Год не может быть пустым");
            return false;
        }

        try {
            int year = StrToInt(Value);
            if (year < 1900 || year > 2100) {
                ShowTempStatus("Год должен быть в диапазоне 1900-2100");
                return false;
            }
        }
        catch (EConvertError &e) {
            ShowTempStatus("Некорректный формат года");
            return false;
        }
    }
    else if (ACol == COL_EMPIRICAL) {
        if (Value.IsEmpty()) {
            ShowTempStatus("Интенсивность не может быть пустой");
            return false;
        }

        try {
            Value = StringReplace(Value, ",", ".", TReplaceFlags() << rfReplaceAll);
            double intensity = StrToFloat(Value);
            if (intensity < 0) {
                ShowTempStatus("Интенсивность не может быть отрицательной");
                return false;
            }
            if (intensity > 429496729.5) {
                ShowTempStatus("Интенсивность слишком большая (макс. 429 млн)");
                return false;
            }
        }
        catch (EConvertError &e) {
            ShowTempStatus("Некорректный числовой формат");
            return false;
        }
    }

    ClearTempStatus();
    return true;
}

//Вычисляет линейную регрессию
TMainForm::TRegressionResults TMainForm::CalculateRegression()
{
	TRegressionResults results = {0};

    int count = 0;
    double sum_t = 0, sum_N = 0, sum_t2 = 0, sum_tN = 0, sum_N2 = 0;

    // Суммирование данных для расчета регрессии
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        String yearStr = StringGrid1->Cells[COL_YEAR][i];
        String intensityStr = StringGrid1->Cells[COL_EMPIRICAL][i];

        if (yearStr.IsEmpty() || intensityStr.IsEmpty()) continue;

        try {
            int year = StrToInt(yearStr);
            intensityStr = StringReplace(intensityStr, ",", ".", TReplaceFlags() << rfReplaceAll);
            double intensity = StrToFloat(intensityStr);

            double t = year - 1980;

            sum_t += t;
            sum_N += intensity;
            sum_t2 += t * t;
            sum_tN += t * intensity;
            sum_N2 += intensity * intensity;
            count++;
        }
        catch (...) {
            continue;
        }
    }

    if (count < 2) {
		ShowTempStatus("Недостаточно данных для расчетов (нужно минимум 2 записи)");
		throw Exception("Недостаточно данных для расчетов. Нужны данные хотя бы за 2 года.");
    }

	//Расчет коэффициентов регрессии
    double denominator = count * sum_t2 - sum_t * sum_t;
    if (denominator == 0) {
        ShowTempStatus("Невозможно вычислить коэффициенты регрессии (деление на 0)");
		throw Exception("Невозможно вычислить коэффициенты регрессии (деление на 0).");
    }

    results.a = (sum_N * sum_t2 - sum_t * sum_tN) / denominator;
    results.b = (count * sum_tN - sum_t * sum_N) / denominator;

	//Расчет статистических показателей
    results.avgN = sum_N / count;
    double variance = (sum_N2 - sum_N * sum_N / count) / (count - 1);
    results.stdDev = sqrt(variance);
    results.correlation = (count * sum_tN - sum_t * sum_N) /
                         sqrt((count * sum_t2 - sum_t * sum_t) * (count * sum_N2 - sum_N * sum_N));
    results.forecast2000 = results.a + results.b * 20;

	//Расчет ошибки аппроксимации
    double sumSquaredErrors = 0;
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        String yearStr = StringGrid1->Cells[COL_YEAR][i];
        String intensityStr = StringGrid1->Cells[COL_EMPIRICAL][i];

        if (yearStr.IsEmpty() || intensityStr.IsEmpty()) continue;

        try {
            int year = StrToInt(yearStr);
            intensityStr = StringReplace(intensityStr, ",", ".", TReplaceFlags() << rfReplaceAll);
            double intensity = StrToFloat(intensityStr);

            double t = year - 1980;
            double theoretical = results.a + results.b * t;
            double error = intensity - theoretical;
            sumSquaredErrors += error * error;
        }
        catch (...) {
            continue;
        }
    }

    double mse = sqrt(sumSquaredErrors / count);
    results.errorPercent = (mse / results.avgN) * 100;

    ShowTempStatus("Расчеты успешно выполнены");
    return results;
}

// Обработчик нажатия клавиш в StringGrid
void __fastcall TMainForm::StringGrid1KeyPress(TObject *Sender, System::WideChar &Key)
{
    TStringGrid *grid = (TStringGrid*)Sender;

    // Блокировка редактирования служебных столбцов
    if (grid->Col == COL_INDEX || grid->Col == COL_THEORETICAL ||
        grid->Col == COL_DEVIATION || grid->Col == COL_ERROR) {
        Key = 0;
        return;
    }

    // Ограничение ввода для года
    if (grid->Col == COL_YEAR) {
        if (!((Key >= '0' && Key <= '9') || Key == 8)) {
            Key = 0;
        }
    }
    // Ограничение ввода для интенсивности
    else if (grid->Col == COL_EMPIRICAL) {
        if (!((Key >= '0' && Key <= '9') || Key == 8 || Key == '.' || Key == ',')) {
            Key = 0;
        }
    }
}
// Создает новый документ
void __fastcall TMainForm::MenuFileNewClick(TObject *Sender)
{
    if (CheckDataModified()) {
        ClearData();
        CurrentFileName = "";
        HasDataChanges = false;

        if (MenuFileSave) {
            MenuFileSave->Enabled = HasDataChanges;
        }
        UpdateStatusBar();
        ShowTempStatus("Создан новый документ");
    }
}

//Открывает существующий файл
void __fastcall TMainForm::MenuFileOpenClick(TObject *Sender)
{
    if (!CheckDataModified()) {
        ShowTempStatus("Открытие файла отменено");
        return;
    }

    try {
        if (OpenDialog->Execute()) {
            String fileName = OpenDialog->FileName;

            if (!FileExists(fileName)) {
                throw Exception("Файл не существует");
            }

            LoadDataFromFile(fileName);
            UpdateStatusBar();
            ShowTempStatus("Данные успешно загружены из файла: " + ExtractFileName(fileName));
        } else {
            ShowTempStatus("Открытие файла отменено");
        }
    }
    catch (Exception &e) {
        String title = L"Интенсивность движения";
        String message;

        if (e.Message == "Файл не существует") {
            String fileName = OpenDialog->FileName;
            message = L"Файл '" + ExtractFileName(fileName) + L"' не существует.\n\n" +
                     L"Проверьте правильность пути и имени файла.";
        } else {
            message = L"Не удалось загрузить файл:\n" + e.Message;
        }

        Application->MessageBox(
            message.w_str(),
            title.w_str(),
            MB_OK | MB_ICONERROR
        );
        ShowTempStatus("Ошибка открытия файла: " + e.Message);
    }
}

//Сохраняет данные в текущий файл
void __fastcall TMainForm::MenuFileSaveClick(TObject *Sender)
{
    try {
        ValidateAllData();

        if (SaveData()) {
            ShowTempStatus("Данные успешно сохранены");
        }
    }
    catch (Exception &e) {
        String errorMessage;
        if (CurrentFileName.IsEmpty()) {
            errorMessage = "Ошибка сохранения:\n" + e.Message;
        } else {
            String fileName = ExtractFileName(CurrentFileName);
            errorMessage = "Ошибка сохранения файла '" + fileName + "':\n" + e.Message;
        }

        ShowTempStatus(errorMessage);

        Application->MessageBox(
            errorMessage.w_str(),
            L"Интенсивность движения",
            MB_OK | MB_ICONERROR
        );
    }
}

//Сохраняет данные в новый файл
void __fastcall TMainForm::MenuFileSaveAsClick(TObject *Sender)
{
    try {
        ValidateAllData();

        if (SaveDataAs()) {
            ShowTempStatus("Данные успешно сохранены");
        }
    }
    catch (Exception &e) {
        String errorMessage;
        if (CurrentFileName.IsEmpty()) {
            errorMessage = "Ошибка сохранения:\n" + e.Message;
        } else {
            String fileName = ExtractFileName(CurrentFileName);
            errorMessage = "Ошибка сохранения файла '" + fileName + "':\n" + e.Message;
        }

        ShowTempStatus(errorMessage);

        Application->MessageBox(
            errorMessage.w_str(),
            L"Интенсивность движения",
            MB_OK | MB_ICONERROR
        );
    }
}

// Выход из приложения
void __fastcall TMainForm::MenuFileExitClick(TObject *Sender)
{
    if (CheckDataModified()) {
        Application->Terminate();
    }
}

//Добавляет новую строку
void __fastcall TMainForm::MenuEditAddClick(TObject *Sender)
{
    StringGrid1->RowCount = StringGrid1->RowCount + 1;
    StringGrid1->Cells[1][StringGrid1->RowCount-1] = "";
    StringGrid1->Cells[2][StringGrid1->RowCount-1] = "";
    SetDataModified(true);

	//Автоматическое заполнение года
    if (StringGrid1->RowCount > 2) {
        int lastYear = 1980;
        for (int i = StringGrid1->RowCount-2; i >= 1; i--) {
            if (!StringGrid1->Cells[1][i].IsEmpty()) {
                try {
                    lastYear = StrToInt(StringGrid1->Cells[1][i]);
                    break;
                }
                catch (...) {
                    continue;
                }
            }
        }
        StringGrid1->Cells[1][StringGrid1->RowCount-1] = IntToStr(lastYear + 1);
    }
    else {
        StringGrid1->Cells[1][1] = "1980";
    }

    UpdateRowNumbers();
    int newRow = StringGrid1->RowCount - 1;
    StringGrid1->Row = newRow;
    StringGrid1->TopRow = Max(0, newRow - (StringGrid1->VisibleRowCount - 1));
    StringGrid1->SetFocus();
    StringGrid1->Col = 2;
    StringGrid1->Invalidate();
    ShowTempStatus("Добавлена новая строка");
}

//Удаляет выбранную строку
void __fastcall TMainForm::MenuEditDeleteClick(TObject *Sender)
{
	DeleteSelectedOrCurrentRow();
}

//Выполнение расчета регрессии
void __fastcall TMainForm::MenuResultsCalculateClick(TObject *Sender)
{
	try {
        ValidateAllData();

        TRegressionResults results = CalculateRegression();

		//Вывод результатов в Memo
        MemoResults->Lines->Clear();
        MemoResults->Lines->Add("=== РЕЗУЛЬТАТЫ РАСЧЕТА ===");
        MemoResults->Lines->Add("Линейная регрессия: N = a + b*t");
        MemoResults->Lines->Add("где t = год - 1980");
        MemoResults->Lines->Add("");
        MemoResults->Lines->Add("Коэффициенты:");
        MemoResults->Lines->Add("a = " + FloatToStrF(results.a, ffFixed, 10, 2));
        MemoResults->Lines->Add("b = " + FloatToStrF(results.b, ffFixed, 10, 2));
        MemoResults->Lines->Add("");
        MemoResults->Lines->Add("Статистика:");
        MemoResults->Lines->Add("Средняя интенсивность: " + FloatToStrF(results.avgN, ffFixed, 10, 2) + " авт./сут.");
        MemoResults->Lines->Add("СКО: " + FloatToStrF(results.stdDev, ffFixed, 10, 2) + " авт./сут.");
        MemoResults->Lines->Add("Коэффициент корреляции: " + FloatToStrF(results.correlation, ffFixed, 10, 4));
        MemoResults->Lines->Add("Ошибка аппроксимации: " + FloatToStrF(results.errorPercent, ffFixed, 10, 2) + "%");
        MemoResults->Lines->Add("");
        MemoResults->Lines->Add("Прогноз на 2000 год: " + FloatToStrF(results.forecast2000, ffFixed, 10, 2) + " авт./сут.");

        DrawRegressionChart(results);
        ShowCalculationTable(results);
    }
    catch (Exception &e) {
        String errorMessage = "Ошибка!\n" + e.Message;
        ShowTempStatus(errorMessage);
        Application->MessageBox(
        errorMessage.w_str(),
        L"Интенсивность движения",
        MB_OK | MB_ICONERROR
        );
        return;
    }
}

//Отрисовка график линейной регрессии
void TMainForm::DrawRegressionChart(const TRegressionResults &results)
{
    Chart1->RemoveAllSeries();
    Chart1->Title->Text->Clear();
	Chart1->Title->Text->Add("                График линейной регрессии интенсивности движения");

	Chart1->LeftAxis->Title->Caption = "Интенсивность, авт./сут.";
    Chart1->BottomAxis->Title->Caption = "Год";

	//Создание серий данных
    TLineSeries *seriesData = new TLineSeries(Chart1);
    seriesData->Title = "Исх. данные";
    seriesData->Color = clBlue;
    seriesData->Pointer->Style = psCircle;
    seriesData->Pointer->Visible = true;

    TLineSeries *seriesRegression = new TLineSeries(Chart1);
    seriesRegression->Title = "Лин. регрессия";
    seriesRegression->Color = clRed;
    seriesRegression->LinePen->Width = 2;

	//Добавление исходных данных
    int minYear = 9999, maxYear = 0;
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        String yearStr = StringGrid1->Cells[COL_YEAR][i];
        String intensityStr = StringGrid1->Cells[COL_EMPIRICAL][i];

        if (yearStr.IsEmpty() || intensityStr.IsEmpty()) continue;

        try {
            int year = StrToInt(yearStr);
            intensityStr = StringReplace(intensityStr, ",", ".", TReplaceFlags() << rfReplaceAll);
            double intensity = StrToFloat(intensityStr);

            seriesData->AddXY(year, intensity, "", clBlue);

            if (year < minYear) minYear = year;
            if (year > maxYear) maxYear = year;
        }
        catch (...) {
            continue;
        }
    }

	//Добавление линии регрессии
    if (minYear != 9999 && maxYear != 0) {
        int startYear = minYear - 1;
        int endYear = maxYear + 3;

        for (int year = startYear; year <= endYear; year++) {
            double t = year - 1980;
            double intensity = results.a + results.b * t;
			seriesRegression->AddXY(year, intensity, year, clRed);
        }

        seriesRegression->AddXY(2000, results.forecast2000, "Прогноз 2000", clGreen);
    }

    Chart1->AddSeries(seriesData);
    Chart1->AddSeries(seriesRegression);
	Chart1->Legend->CustomPosition = true;
	Chart1->Legend->Left = 10;
	Chart1->Legend->Top = Chart1->Height - 310;

    ShowTempStatus("График построен");
}

//Показывает таблицу расчетов
void TMainForm::ShowCalculationTable(const TRegressionResults &results)
{
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        StringGrid1->Cells[COL_THEORETICAL][i] = "";
        StringGrid1->Cells[COL_DEVIATION][i] = "";
        StringGrid1->Cells[COL_ERROR][i] = "";

        String yearStr = StringGrid1->Cells[COL_YEAR][i];
        String intensityStr = StringGrid1->Cells[COL_EMPIRICAL][i];

        if (yearStr.IsEmpty() || intensityStr.IsEmpty()) continue;

        try {
            int year = StrToInt(yearStr);
            intensityStr = StringReplace(intensityStr, ",", ".", TReplaceFlags() << rfReplaceAll);
            double empirical = StrToFloat(intensityStr);

            double t = year - 1980;
            double theoretical = results.a + results.b * t;
            double deviation = empirical - theoretical;
            double relativeError = (deviation / empirical) * 100;

            StringGrid1->Cells[COL_THEORETICAL][i] = FloatToStrF(theoretical, ffFixed, 10, 2);
            StringGrid1->Cells[COL_DEVIATION][i] = FloatToStrF(deviation, ffFixed, 10, 2);
            StringGrid1->Cells[COL_ERROR][i] = FloatToStrF(relativeError, ffFixed, 10, 2);
        }
        catch (...) {
            continue;
        }
    }

    StringGrid1->Invalidate();
    ShowTempStatus("Расчеты завершены. Результаты добавлены в таблицу");
}

//Сохраняет результаты расчетов
void __fastcall TMainForm::MenuResultsSaveClick(TObject *Sender)
{
    bool hasResults = false;
    String resultsText = MemoResults->Lines->Text.Trim();

    if (resultsText.Length() > 0) {
        for (int i = 0; i < MemoResults->Lines->Count; i++) {
            String line = MemoResults->Lines->Strings[i].Trim();
            if (line.Length() > 0 &&
               (line.Pos("=") > 0 || line.Pos(":") > 0 ||
                line.Pos("авт./сут.") > 0 || line.Pos("%") > 0)) {
                hasResults = true;
                break;
            }
        }
    }

    bool hasTableData = false;
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        if (!StringGrid1->Cells[COL_THEORETICAL][i].IsEmpty() ||
            !StringGrid1->Cells[COL_DEVIATION][i].IsEmpty() ||
            !StringGrid1->Cells[COL_ERROR][i].IsEmpty()) {
            hasTableData = true;
            break;
        }
    }

    if (!hasResults && !hasTableData) {
        ShowTempStatus("Нет данных для сохранения результатов");
        Application->MessageBox(
            L"Нет данных для сохранения. Сначала выполните расчеты.",
            L"Интенсивность движения",
            MB_OK | MB_ICONWARNING
        );
        return;
    }

    SaveResultsDialog->DefaultExt = "txt";
    SaveResultsDialog->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
    SaveResultsDialog->FileName = "";

    if (SaveResultsDialog->Execute()) {
        try {
            String fileName = SaveResultsDialog->FileName;
            String ext = ExtractFileExt(fileName);
            if (ext.IsEmpty() || !SameText(ext, ".txt")) {
                if (ext.IsEmpty()) {
                    fileName = fileName + ".txt";
                } else {
                    fileName = ChangeFileExt(fileName, ".txt");
                }
            }

            TStringList *saveLines = new TStringList();

            try {
				if (hasResults) {
                    for (int i = 0; i < MemoResults->Lines->Count; i++) {
                        saveLines->Add(MemoResults->Lines->Strings[i]);
                    }
                    saveLines->Add("");
                }

                saveLines->Add("=== ТАБЛИЧНЫЕ ДАННЫЕ ===");
				saveLines->Add("");
                saveLines->Add(FormatTableForExport());
                saveLines->SaveToFile(fileName);

				String successMessage = "Результаты успешно сохранены в файл:\n" + fileName;
                if (hasResults) {
                    successMessage = successMessage + "\n\nСохранено:";
					if (hasResults) successMessage = successMessage + "\n- Результаты расчетов";
                    if (hasTableData) successMessage = successMessage + "\n- Данные таблицы";
                }

                ShowTempStatus("Результаты успешно сохранены в файл: " + ExtractFileName(fileName));

//                Application->MessageBox(
//					successMessage.w_str(),
//					L"Интенсивность движения",
//					MB_OK | MB_ICONINFORMATION
//				);
            }
            __finally {
                delete saveLines;
            }
        }
        catch (Exception &e) {
            String errorMessage = "Ошибка сохранения результатов: " + e.Message;
            ShowTempStatus(errorMessage);

            Application->MessageBox(
                errorMessage.w_str(),
                L"Интенсивность движения",
                MB_OK | MB_ICONERROR
            );
        }
    } else {
        ShowTempStatus("Сохранение результатов отменено");
    }
}

//Форматирует таблицу для экспорта
String TMainForm::FormatTableForExport()
{
    TStringList *result = new TStringList();
    try {
        int colWidths[6] = {0, 0, 0, 0, 0, 0};
		//Определение максимальной ширины для каждого столбца
        for (int col = 0; col < 6; col++) {
            int maxWidth = StringGrid1->Cells[col][0].Length();
            for (int row = 1; row < StringGrid1->RowCount; row++) {
                if (StringGrid1->Cells[col][row].Length() > maxWidth) {
                    maxWidth = StringGrid1->Cells[col][row].Length();
                }
            }
            colWidths[col] = maxWidth + 2;
        }

        // Вспомогательные лямбда-функции для форматирования
        auto PadStringLeft = [](const String &str, int width) -> String {
            String result = str;
            while (result.Length() < width) {
                result = result + " ";
            }
            return result;
        };

        auto PadStringRight = [](const String &str, int width) -> String {
            String result = str;
            while (result.Length() < width) {
                result = " " + result;
            }
            return result;
        };

        auto RepeatChar = [](wchar_t ch, int count) -> String {
            String result = "";
            for (int i = 0; i < count; i++) {
                result = result + ch;
            }
            return result;
        };

        // Формирование заголовка
        String header = "";
        for (int col = 0; col < 6; col++) {
            String cell = StringGrid1->Cells[col][0];
            header = header + PadStringLeft(cell, colWidths[col]);
        }
        result->Add(header);

        // Разделитель
        String separator = "";
        for (int col = 0; col < 6; col++) {
            separator = separator + RepeatChar('-', colWidths[col]);
        }
        result->Add(separator);

		//Данные таблицы
        for (int row = 1; row < StringGrid1->RowCount; row++) {
            String yearStr = StringGrid1->Cells[COL_YEAR][row];
            String empiricalStr = StringGrid1->Cells[COL_EMPIRICAL][row];
            if (yearStr.IsEmpty() && empiricalStr.IsEmpty()) continue;

            String line = "";
            for (int col = 0; col < 6; col++) {
                String cell = StringGrid1->Cells[col][row];

                if (col == COL_INDEX || col == COL_YEAR ||
                    col == COL_EMPIRICAL || col == COL_THEORETICAL ||
                    col == COL_DEVIATION || col == COL_ERROR) {
                    line = line + PadStringRight(cell, colWidths[col]);
                }
                else {
                    line = line + PadStringLeft(cell, colWidths[col]);
                }
            }
            result->Add(line);
        }

        return result->Text;
    }
    __finally {
        delete result;
    }
}

//Обработчики кнопок панели инструментов
void __fastcall TMainForm::ButtonAddClick(TObject *Sender)
{
    MenuEditAddClick(Sender);
}

void __fastcall TMainForm::ButtonDeleteClick(TObject *Sender)
{
	DeleteSelectedOrCurrentRow();
}

void __fastcall TMainForm::ButtonCalculateClick(TObject *Sender)
{
    MenuResultsCalculateClick(Sender);
}

// Сохраняет график в файл
void __fastcall TMainForm::MenuResultsSaveChartClick(TObject *Sender)
{
    if (Chart1->SeriesCount() == 0) {
        ShowTempStatus("Нет данных для сохранения графика");
        Application->MessageBox(
            L"Нет данных для сохранения. Сначала выполните расчеты.",
            L"Интенсивность движения",
            MB_OK | MB_ICONWARNING
        );
        return;
    }

    TSaveDialog* saveDialog = new TSaveDialog(this);
    saveDialog->Filter = "PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp|JPEG files (*.jpg)|*.jpg|EMF files (*.emf)|*.emf";
    saveDialog->DefaultExt = "png";
    saveDialog->Title = "Сохранение графика — Интенсивность движения";

    if (!saveDialog->Execute()) {
        delete saveDialog;
        ShowTempStatus("Сохранение графика отменено");
        return;
	}

    String fileName = saveDialog->FileName;
    String ext = LowerCase(ExtractFileExt(fileName));
    bool success = false;

    try {
        // Сохранение в PNG
        if (ext == ".png") {
            TPngImage* png = new TPngImage();
            try {
                TBitmap* bmp = new TBitmap();
                try {
                    bmp->SetSize(Chart1->Width, Chart1->Height);
                    Chart1->PaintTo(bmp->Canvas, 0, 0);
                    png->Assign(bmp);
                    png->SaveToFile(fileName);
                    success = true;
                }
                __finally {
                    delete bmp;
                }
            }
            __finally {
                delete png;
            }
        }
		//Сохранение в JPEG
        else if (ext == ".jpg" || ext == ".jpeg") {
            TJPEGImage* jpg = new TJPEGImage();
            try {
                TBitmap* bmp = new TBitmap();
                try {
                    bmp->SetSize(Chart1->Width, Chart1->Height);
                    Chart1->PaintTo(bmp->Canvas, 0, 0);
                    jpg->Assign(bmp);
                    jpg->CompressionQuality = 90;
                    jpg->SaveToFile(fileName);
                    success = true;
                }
                __finally {
                    delete bmp;
                }
            }
            __finally {
                delete jpg;
            }
        }
		//Сохранение в BMP
        else if (ext == ".bmp") {
            TBitmap* bmp = new TBitmap();
            try {
                bmp->SetSize(Chart1->Width, Chart1->Height);
                Chart1->PaintTo(bmp->Canvas, 0, 0);
                bmp->SaveToFile(fileName);
                success = true;
            }
            __finally {
                delete bmp;
            }
        }
		//Сохранение в EMF
        else if (ext == ".emf") {
            TMetafile* metafile = new TMetafile();
            try {
                metafile->Width = Chart1->Width;
                metafile->Height = Chart1->Height;

                TMetafileCanvas* mfCanvas = new TMetafileCanvas(metafile, 0);
                try {
                    Chart1->PaintTo(mfCanvas, 0, 0);
                }
                __finally {
                    delete mfCanvas;
                }

                metafile->SaveToFile(fileName);
                success = true;
            }
            __finally {
                delete metafile;
            }
        }
		//Сохранение по умолчанию
        else {
            TBitmap* bmp = new TBitmap();
            try {
                bmp->SetSize(Chart1->Width, Chart1->Height);
                Chart1->PaintTo(bmp->Canvas, 0, 0);
                bmp->SaveToFile(fileName);
                success = true;
            }
            __finally {
                delete bmp;
            }
        }

        if (success) {
            String msg = "График успешно сохранён в файл: " + fileName;
            ShowTempStatus(msg);
//            Application->MessageBox(
//                msg.w_str(),
//                L"Интенсивность движения",
//                MB_OK | MB_ICONINFORMATION
//            );
        }
    }
    catch (Exception& e) {
        String msg = "Ошибка сохранения графика: " + e.Message;
        ShowTempStatus(msg);
        Application->MessageBox(
            msg.w_str(),
            L"Интенсивность движения",
            MB_OK | MB_ICONERROR
        );
    }

    delete saveDialog;
}

//Обработчик нажатия клавиш в таблице
void __fastcall TMainForm::StringGrid1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    TStringGrid *grid = (TStringGrid*)Sender;

    // Комбинация Ctrl+C - копирование
    if (Key == 'C' && Shift.Contains(ssCtrl)) {
        if (grid->Row > 0 && (grid->Col == COL_YEAR || grid->Col == COL_EMPIRICAL)) {
            String cellText = grid->Cells[grid->Col][grid->Row];
            if (!cellText.IsEmpty()) {
                Clipboard()->AsText = cellText;
                ShowTempStatus("Ячейка скопирована");
            } else {
                ShowTempStatus("Ячейка пустая");
            }
            Key = 0;
            return;
        }
    }
	//Комбинация Ctrl+V - вставка
    else if (Key == 'V' && Shift.Contains(ssCtrl)) {
        if (grid->Row > 0 && (grid->Col == COL_YEAR || grid->Col == COL_EMPIRICAL)) {
            if (Clipboard()->HasFormat(CF_TEXT)) {
                String clipboardText = Clipboard()->AsText.Trim();
                if (ValidateCell(grid->Col, grid->Row, clipboardText)) {
                    grid->Cells[grid->Col][grid->Row] = clipboardText;
                    SetDataModified(true);
                    grid->Invalidate();
                    ShowTempStatus("Данные вставлены");
                } else {
                    ShowTempStatus("Некорректные данные для вставки");
                }
            }
            Key = 0;
            return;
        }
    }
	//Комбинация Ctrl+X - вырезание
    else if (Key == 'X' && Shift.Contains(ssCtrl)) {
        if (grid->Row > 0 && (grid->Col == COL_YEAR || grid->Col == COL_EMPIRICAL)) {
            String cellText = grid->Cells[grid->Col][grid->Row];
            if (!cellText.IsEmpty()) {
                Clipboard()->AsText = cellText;
                grid->Cells[grid->Col][grid->Row] = "";
                SetDataModified(true);
                grid->Invalidate();
                ShowTempStatus("Ячейка вырезана");
            } else {
                ShowTempStatus("Ячейка пустая");
            }
            Key = 0;
            return;
        }
    }
	//Комбинация Ctrl+A - выделение всех
    else if (Key == 'A' && Shift.Contains(ssCtrl)) {
        TGridRect selection;
        selection.Left = COL_YEAR;
        selection.Right = COL_EMPIRICAL;
        selection.Top = 1;
        selection.Bottom = grid->RowCount - 1;

        grid->Selection = selection;
        Key = 0;
        return;
    }

	//Обработка ввода данных
    else if ((Key >= 32 && Key <= 126) ||
             (Key >= '0' && Key <= '9') ||
             (Key >= 'A' && Key <= 'Z') ||
             (Key >= VK_NUMPAD0 && Key <= VK_NUMPAD9) ||
             Key == VK_DECIMAL || Key == VK_OEM_COMMA || Key == VK_OEM_PERIOD) {
        if (grid->Row > 0 &&
            (grid->Col == COL_YEAR || grid->Col == COL_EMPIRICAL)) {
            if (!grid->Options.Contains(goEditing)) {
                grid->Options = grid->Options << goEditing;
            }
        }
    }
}

//Обработчик двойного щелчка
void __fastcall TMainForm::StringGrid1DblClick(TObject *Sender)
{
    TStringGrid *grid = (TStringGrid*)Sender;

    if (grid->Row > 0 &&
        (grid->Col == COL_YEAR || grid->Col == COL_EMPIRICAL)) {

        grid->Options = grid->Options << goEditing;

        if (grid->CanFocus()) {
            grid->SetFocus();
        }
    }
}

// Обработчик нажатия мыши
void __fastcall TMainForm::StringGrid1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TStringGrid *grid = (TStringGrid*)Sender;
    int ACol, ARow;
    grid->MouseToCell(X, Y, ACol, ARow);

    if (Button == mbLeft) {
        if (Shift.Contains(ssShift)) {
            TGridRect selection = grid->Selection;
            selection.Bottom = ARow;
            grid->Selection = selection;
        }
        else {
            if (ARow > 0 && (ACol == COL_YEAR || ACol == COL_EMPIRICAL)) {
                grid->Col = ACol;
                grid->Row = ARow;
                if (!grid->Options.Contains(goEditing)) {
                    grid->Options = grid->Options << goEditing;
                }
            } else {
                if (grid->Options.Contains(goEditing)) {
                    grid->Options = grid->Options >> goEditing;
                }
            }
		}
	}
}

// Удаляет выбранный диапазон строк
void TMainForm::DeleteSelectedRange()
{
    TGridRect selection = StringGrid1->Selection;
    int startRow = Min(selection.Top, selection.Bottom);
	int endRow = Max(selection.Top, selection.Bottom);
    if (startRow <= 0) startRow = 1;
    if (endRow <= 0 || startRow >= StringGrid1->RowCount) {
        ShowTempStatus("Нет данных для удаления");
        return;
    }

	int selectedCount = endRow - startRow + 1;
    int totalDataRowsBefore = 0;
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        if (!StringGrid1->Cells[COL_YEAR][i].IsEmpty() ||
            !StringGrid1->Cells[COL_EMPIRICAL][i].IsEmpty()) {
            ++totalDataRowsBefore;
        }
    }

	int selectedDataRows = 0;
    for (int row = startRow; row <= endRow; ++row) {
        if (!StringGrid1->Cells[COL_YEAR][row].IsEmpty() ||
            !StringGrid1->Cells[COL_EMPIRICAL][row].IsEmpty()) {
            ++selectedDataRows;
        }
    }

    bool willBeEmpty = (totalDataRowsBefore - selectedDataRows == 0);

    String message;
    if (selectedCount == 1)
    {
        String year = StringGrid1->Cells[COL_YEAR][startRow];
        String intensity = StringGrid1->Cells[COL_EMPIRICAL][startRow];
        message = L"Удалить строку?\n";
        message += L"Год: " + (year.IsEmpty() ? L"—" : year) + L"\n";
        message += L"Интенсивность: " + (intensity.IsEmpty() ? L"—" : intensity + L" авт./сут.");
    }
    else
    {
        message = L"Вы действительно хотите удалить " + IntToStr(selectedCount) + L" строк(у)?\n";
        if (selectedDataRows > 0) {
            message += L"Данное действие нельзя отменить.";
        } else {
            message += L"Все выделенные строки пустые.";
        }
    }

    if (willBeEmpty && selectedDataRows > 0) {
		message += L"\n\nПосле удаления таблица станет пустой.";
    }

    int result = Application->MessageBox(
        message.c_str(),
		L"Интенсивность движения",
        MB_YESNO | MB_ICONQUESTION
    );

    if (result != IDYES) {
        return;
	}
    int rowsToDelete = endRow - startRow + 1;
    int newRowCount = StringGrid1->RowCount - rowsToDelete;

    for (int i = startRow; i < newRowCount; i++) {
        for (int col = 0; col < StringGrid1->ColCount; col++) {
            StringGrid1->Cells[col][i] = StringGrid1->Cells[col][i + rowsToDelete];
        }
    }

    for (int i = newRowCount; i < StringGrid1->RowCount; i++) {
		for (int col = 0; col < StringGrid1->ColCount; col++) {
            StringGrid1->Cells[col][i] = "";
        }
    }

	StringGrid1->RowCount = newRowCount;
    UpdateRowNumbers();
    SetDataModified(true);
	UpdateStatusBar();

    int newRow = (startRow <= StringGrid1->RowCount - 1) ? startRow : StringGrid1->RowCount - 1;
	if (newRow < 1) newRow = 1;

	StringGrid1->Row = newRow;
	StringGrid1->Col = COL_EMPIRICAL;
	StringGrid1->TopRow = Max(0, newRow - (StringGrid1->VisibleRowCount - 1));
	StringGrid1->SetFocus();
}

//Вставляет строку в текущую позицию курсора
void TMainForm::InsertRowAtCursor()
{
    int currentRow = StringGrid1->Row;
	if (currentRow <= 0) {
		currentRow = StringGrid1->RowCount - 1;
    }

	int newRow = currentRow + 1;
	StringGrid1->RowCount++;
    for (int row = StringGrid1->RowCount - 1; row > newRow; row--) {
        for (int col = 1; col < StringGrid1->ColCount; col++) {
            StringGrid1->Cells[col][row] = StringGrid1->Cells[col][row - 1];
        }
    }

    for (int col = 1; col < StringGrid1->ColCount; col++) {
        StringGrid1->Cells[col][newRow] = "";
    }

	RecalculateYears();
	UpdateRowNumbers();
    StringGrid1->Row = newRow;
    StringGrid1->Col = COL_EMPIRICAL;
    StringGrid1->TopRow = Max(0, newRow - (StringGrid1->VisibleRowCount - 1));
    StringGrid1->SetFocus();

	SetDataModified(true);
    StringGrid1->Invalidate();
    ShowTempStatus("Вставлена новая строка после позиции " + IntToStr(currentRow));
}

// Обработчик перетаскивания над таблицей
void __fastcall TMainForm::StringGrid1DragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    Accept = (Source == StringGrid1);
}

// Обработчик завершения перетаскивания
void __fastcall TMainForm::StringGrid1DragDrop(TObject *Sender, TObject *Source, int X, int Y)
{
    if (Source != StringGrid1) return;

    int ACol, ARow;
    StringGrid1->MouseToCell(X, Y, ACol, ARow);

    int FromRow = StringGrid1->Selection.Top;

    if (FromRow > 0 && ARow > 0 && FromRow != ARow) {
        TStringList *tempData = new TStringList();
        try {
            // Сохранение данных исходной строки
            for (int col = 0; col < StringGrid1->ColCount; col++) {
                tempData->Add(StringGrid1->Cells[col][FromRow]);
            }

            // Перемещение строк
            if (ARow > FromRow) {
                // Вниз
                for (int row = FromRow; row < ARow; row++) {
                    for (int col = 0; col < StringGrid1->ColCount; col++) {
                        StringGrid1->Cells[col][row] = StringGrid1->Cells[col][row + 1];
                    }
                }
            } else {
                // Вверх
                for (int row = FromRow; row > ARow; row--) {
                    for (int col = 0; col < StringGrid1->ColCount; col++) {
                        StringGrid1->Cells[col][row] = StringGrid1->Cells[col][row - 1];
                    }
                }
            }

            // Вставка данных
            for (int col = 0; col < StringGrid1->ColCount; col++) {
                StringGrid1->Cells[col][ARow] = tempData->Strings[col];
            }

			UpdateRowNumbers();
			RecalculateYears();
            SetDataModified(true);
            StringGrid1->Row = ARow;
            ShowTempStatus("Строка перемещена с позиции " + IntToStr(FromRow) +
                          " на позицию " + IntToStr(ARow));
		}
        __finally {
            delete tempData;
        }
    }
}

void TMainForm::RecalculateYears()
{
    if (StringGrid1->RowCount <= 1) return;

    int baseYear = -1;
	//первый непустой год
    for (int i = 1; i < StringGrid1->RowCount; i++) {
        String yearStr = StringGrid1->Cells[COL_YEAR][i];
        if (!yearStr.IsEmpty()) {
            try {
                baseYear = StrToInt(yearStr);
                break;
            } catch (...) {}
        }
	}
	if (baseYear == -1) baseYear = 1980;

	//заполнение года по порядку
    for (int i = 1; i < StringGrid1->RowCount; i++) {
		StringGrid1->Cells[COL_YEAR][i] = IntToStr(baseYear + (i - 1));
	}
}

void TMainForm::DeleteSelectedOrCurrentRow()
{
    TGridRect sel = StringGrid1->Selection;

	// Если выделение некорректно, используем текущую строку
    if (sel.Top <= 0 || sel.Bottom <= 0 || sel.Top > sel.Bottom)
    {
        int currentRow = StringGrid1->Row;
        if (currentRow <= 0)
        {
            ShowTempStatus("Выберите строку для удаления");
            Application->MessageBox(
				L"Выберите строку для удаления",
                L"Интенсивность движения",
                MB_OK | MB_ICONWARNING
            );
            return;
        }

        // Создаём выделение одной строки
        TGridRect newSel;
        newSel.Left = 0;
        newSel.Right = StringGrid1->ColCount - 1;
        newSel.Top = newSel.Bottom = currentRow;
        StringGrid1->Selection = newSel;
    }
	DeleteSelectedRange();
}

bool __fastcall TMainForm::StringGrid1SelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect)
{
	CanSelect = (ARow > 0) && (ACol > 0);
    return true;
}

// Обработчик закрытия формы
void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = CheckDataModified();
}

void __fastcall TMainForm::ButtonInsertClick(TObject *Sender)
{
    InsertRowAtCursor();
}

