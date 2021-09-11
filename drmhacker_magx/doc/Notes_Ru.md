Реверс исполнительного файла `setupprofile` с целью нахождения правильных аргументов и вызовов функций для снятия защиты с MP3-файлов.

Похоже что функция `MP_CreateRingtoneEngine()` используется лишь для воспроизведения некого `ui_volume.wav` при каких-то условиях. Хотя есть сомнения по этому поводу. Возможно там всё-таки производится инициализация движка медиаплеера. Может быть это что-то типа звукового оповещания при изменении уровней громкости?

Пробуем посмотреть Media File Picker'ы и найти библиотеку, которые их содержат.

Кандидаты:
1. libezx_mp_playerengine.so.1 (-)
2. libezxmystuff-play.so.1 (-)
3. libmfsvc.so.1 (+)
4. libezxappbase.so.1 (-)

Используется функция `MediaFilePicker::pickRingtone()` для выбора музыки в библиотеке *libmfsvc.so.1*, похоже получение виртуального пути для плеера находится именно здесь, так как это библиотека зависит от DRM-либы в т. ч.

Идея №1, похоже что функция `DRM_StartRightsMeter()` принимает в качестве аргумента, который следует за путём к файлу параметр действия (play, print, display и др.), возможно следует поиграться именно с предпоследним параметром функции. Кроме того, возможно последнего параметра вовсе нет, поэтому с ним тоже следует поиграться.

```diff
- const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 0, 1);
+ const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 0, 5);
```

```diff
- const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 0, 1);
+ const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 5);
```

Возможно значения action-параметра следующие:

```java
public static final int DRM_PERMISSION_PLAY = 1;
public static final int DRM_PERMISSION_DISPLAY = 2;
public static final int DRM_PERMISSION_EXECUTE = 3;
public static final int DRM_PERMISSION_PRINT = 4;
```

Идея №2, использовать структурку вида:

```diff
struct DRM_METADATA_T {
    uint8_t contentID[512];
    uint8_t contentType[256];
    uint8_t rightsIssuerURL[1024];
    uint32_t contentLen;
};
```

На функции `DRM_GetContentMetaDataInFile()` вызвав её первой. Затем следует проверить все сдампленные значения в ней.

Идея №3, выставить функцию `DRM_PreExtern()` перед всеми манипуляциями.

Идея №4, изучить ap log, файлики и директории `/mnt/msc_int0/logger`, `/usr/bin/aplogd`, как именно читать эти логи?

Пример использования похожих функций, которые удалось нагуглить:

https://github.com/rajdeokumarsingh/Notes/blob/master/computer.science/android/multimedia/drm/drm.pekall.2.2/concept.drm.txt

https://github.com/rajdeokumarsingh/Notes/blob/master/computer.science/android/multimedia/drm/drm.pekall.2.2/flow/flow.play.cpp

Но похоже это не имеет ничего общего с реальностью.
