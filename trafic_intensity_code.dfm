object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = #1048#1085#1090#1077#1085#1089#1080#1074#1085#1086#1089#1090#1100' '#1076#1074#1080#1078#1077#1085#1080#1103
  ClientHeight = 648
  ClientWidth = 1060
  Color = clBtnFace
  Constraints.MinHeight = 500
  Constraints.MinWidth = 750
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Menu = MainMenu
  Position = poDesktopCenter
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  TextHeight = 15
  object StatusBar: TStatusBar
    Left = 0
    Top = 629
    Width = 1060
    Height = 19
    Panels = <>
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1060
    Height = 41
    Align = alTop
    TabOrder = 0
    DesignSize = (
      1060
      41)
    object ButtonAdd: TButton
      Left = 35
      Top = 11
      Width = 145
      Height = 25
      Anchors = [akTop, akBottom]
      Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1076#1072#1085#1085#1099#1077' '#1079#1072' '#1075#1086#1076
      TabOrder = 0
      OnClick = ButtonAddClick
    end
    object ButtonCalculate: TButton
      Left = 886
      Top = 10
      Width = 145
      Height = 25
      Anchors = [akTop, akBottom]
      Caption = #1056#1072#1089#1089#1095#1080#1090#1072#1090#1100
      TabOrder = 3
      OnClick = ButtonCalculateClick
    end
    object ButtonDelete: TButton
      Left = 311
      Top = 10
      Width = 145
      Height = 25
      Anchors = [akTop, akBottom]
      Caption = #1059#1076#1072#1083#1080#1090#1100' '#1076#1072#1085#1085#1099#1077' '#1079#1072' '#1075#1086#1076'('#1072')'
      TabOrder = 1
      OnClick = ButtonDeleteClick
    end
    object ButtonInsert: TButton
      Left = 590
      Top = 10
      Width = 153
      Height = 25
      Anchors = [akTop, akBottom]
      Caption = #1042#1089#1090#1072#1074#1080#1090#1100' '#1076#1072#1085#1085#1099#1077' '#1079#1072' '#1075#1086#1076
      TabOrder = 2
      OnClick = ButtonInsertClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 41
    Width = 1060
    Height = 588
    Align = alClient
    TabOrder = 1
    object Splitter2: TSplitter
      Left = 585
      Top = 1
      Height = 586
      Color = clSilver
      ParentColor = False
      ResizeStyle = rsUpdate
      ExplicitLeft = 576
      ExplicitTop = 272
      ExplicitHeight = 100
    end
    object Panel3: TPanel
      Left = 1
      Top = 1
      Width = 584
      Height = 586
      Align = alLeft
      TabOrder = 0
      object Label1: TLabel
        Left = 1
        Top = 1
        Width = 582
        Height = 25
        Align = alTop
        Caption = #1044#1072#1085#1085#1099#1077' '#1080#1085#1090#1077#1085#1089#1080#1074#1085#1086#1089#1090#1080' '#1076#1074#1080#1078#1077#1085#1080#1103
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -21
        Font.Name = 'Goudy Old Style'
        Font.Style = []
        ParentFont = False
        ExplicitWidth = 335
      end
      object StringGrid1: TStringGrid
        Left = 1
        Top = 26
        Width = 582
        Height = 559
        Align = alClient
        DragMode = dmAutomatic
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goRowMoving, goEditing, goFixedRowDefAlign]
        TabOrder = 0
        OnDblClick = StringGrid1DblClick
        OnDragDrop = StringGrid1DragDrop
        OnDragOver = StringGrid1DragOver
        OnKeyDown = StringGrid1KeyDown
        OnKeyPress = StringGrid1KeyPress
        OnMouseDown = StringGrid1MouseDown
      end
    end
    object Panel4: TPanel
      Left = 588
      Top = 1
      Width = 471
      Height = 586
      Align = alClient
      Anchors = []
      TabOrder = 1
      object Splitter1: TSplitter
        Left = 1
        Top = 344
        Width = 469
        Height = 3
        Cursor = crVSplit
        Align = alBottom
        Color = clActiveBorder
        ParentColor = False
        ResizeStyle = rsUpdate
        ExplicitTop = 1
        ExplicitWidth = 584
      end
      object Panel5: TPanel
        Left = 1
        Top = 1
        Width = 469
        Height = 343
        Align = alClient
        Anchors = []
        Caption = 'Panel5'
        TabOrder = 0
        object Label3: TLabel
          Left = 1
          Top = 1
          Width = 467
          Height = 16
          Align = alTop
          Caption = #1043#1088#1072#1092#1080#1082' '#1083#1080#1085#1077#1081#1085#1086#1081' '#1088#1077#1075#1088#1077#1089#1089#1080#1080
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Goudy Old Style'
          Font.Style = []
          ParentFont = False
          ExplicitWidth = 187
        end
        object Chart1: TChart
          Left = 1
          Top = 17
          Width = 467
          Height = 325
          Title.Text.Strings = (
            'TChart')
          Align = alClient
          TabStop = False
          TabOrder = 0
          DefaultCanvas = 'TGDIPlusCanvas'
          ColorPaletteIndex = 13
        end
      end
      object Panel6: TPanel
        Left = 1
        Top = 347
        Width = 469
        Height = 238
        Align = alBottom
        Anchors = []
        Caption = 'Panel6'
        TabOrder = 1
        object Label2: TLabel
          Left = 1
          Top = 1
          Width = 467
          Height = 16
          Align = alTop
          Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090#1099' '#1088#1072#1089#1095#1105#1090#1086#1074
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Goudy Old Style'
          Font.Style = []
          ParentFont = False
          ExplicitWidth = 144
        end
        object MemoResults: TMemo
          Left = 1
          Top = 17
          Width = 467
          Height = 220
          Align = alClient
          Color = cl3DLight
          Lines.Strings = (
            '')
          ReadOnly = True
          ScrollBars = ssVertical
          TabOrder = 0
        end
      end
    end
  end
  object MainMenu: TMainMenu
    Left = 728
    Top = 640
    object MenuFile: TMenuItem
      Caption = #1060#1072#1081#1083
      object MenuFileNew: TMenuItem
        Caption = #1057#1086#1079#1076#1072#1090#1100
        OnClick = MenuFileNewClick
      end
      object MenuFileOpen: TMenuItem
        Caption = #1054#1090#1082#1088#1099#1090#1100'...'
        OnClick = MenuFileOpenClick
      end
      object MenuFileSave: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
        OnClick = MenuFileSaveClick
      end
      object MenuFileSaveAs: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1082#1072#1082'...'
        OnClick = MenuFileSaveAsClick
      end
      object Separator1: TMenuItem
        Caption = '-'
      end
      object MenuFileExit: TMenuItem
        Caption = #1042#1099#1093#1086#1076
        OnClick = MenuFileExitClick
      end
    end
    object MenuEdit: TMenuItem
      Caption = #1056#1077#1076#1072#1082#1090#1080#1088#1086#1074#1072#1085#1080#1077
      object MenuEditAdd: TMenuItem
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1076#1072#1085#1085#1099#1077' '#1079#1072' '#1075#1086#1076
        OnClick = MenuEditAddClick
      end
      object MenuEditDelete: TMenuItem
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1076#1072#1085#1085#1099#1077' '#1079#1072' '#1075#1086#1076'('#1072')'
        OnClick = MenuEditDeleteClick
      end
    end
    object MenuResults: TMenuItem
      Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090#1099
      object MenuResultsCalculate: TMenuItem
        Caption = #1056#1072#1089#1089#1095#1080#1090#1072#1090#1100
        OnClick = MenuResultsCalculateClick
      end
      object MenuResultsSave: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1088#1077#1079#1091#1083#1100#1090#1072#1090#1099'...'
        OnClick = MenuResultsSaveClick
      end
      object MenuResultsSaveChart: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1075#1088#1072#1092#1080#1082'...'
        OnClick = MenuResultsSaveChartClick
      end
    end
  end
  object OpenDialog: TOpenDialog
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 16
    Top = 640
  end
  object SaveDialog: TSaveDialog
    Options = [ofHideReadOnly, ofExtensionDifferent, ofEnableSizing]
    Left = 80
    Top = 640
  end
  object SaveResultsDialog: TSaveDialog
    Left = 144
    Top = 640
  end
end
