# WidgetMarkup

**English** | [简体中文](README.zh-cn.md)

Build UMG widget trees with XML in Unreal Engine.

![](Documents/WidgetMarkup.gif)

## Usage

In the Unreal Editor, enter the following in the CMD input bar:

```
WidgetMarkup.Show <*.unrealwidgetmarkup File Path>
```

Example file content:

```xml
<WidgetBlueprint>
  <WidgetTree>
    <CanvasPanel>
      <TextBlock Text="Text Block in Canvas Panel" Slot.LayoutData.Anchors.Minimum="0.5,0.5" Slot.LayoutData.Anchors.Maximum="0.5,0.5" Slot.bAutoSize="True" Slot.LayoutData.Alignment="0.5,0.5" />
    </CanvasPanel>
  </WidgetTree>
</WidgetBlueprint>
```

A window will open with a live preview of the corresponding UI:

![Text Block in Canvas Panel](Documents/TextBlockInCanvasPanel.png)

## Language Extension

This project also provides a [VSCode extension](https://github.com/9087/Unreal.WidgetMarkup.VSCodeExtension) with basic syntax checking and IntelliSense.

Before using the extension, run the following in the Editor CMD input bar:

```
WidgetMarkup.GenerateIntellisenseData <IntelliSense Data Path>
```

Then configure the `IntelliSense Data Path` in the extension settings:

![Extension Settings](Documents/ExtensionSettings.png)

Open `*.unrealwidgetmarkup` files in VSCode to use the extension:

![VSCode Extension](Documents/Extension.png)
