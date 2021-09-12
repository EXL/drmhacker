Похоже что расшифрованный файлик появляется в директории `/mnt/drmfs/` из которой уже приложением-клиентом он читается без проблем. Если с картинками прокатывает обычный `DRM_StartRightsMeter()`, то вот с MP3-файлами использовать его что-то не получается. Функция возвращает значение `0x7D0` вместо требуемого `0x7D2`, вероятно этим обозначается какия-то ошибка.

Реверс-инжиниринг исполняемого файла `setupprofile` с целью нахождения правильных аргументов и вызовов функций для снятия защиты с MP3-файлов.

Похоже что функция `MP_CreateRingtoneEngine()` (видимо конструктор) используется лишь для воспроизведения некого `ui_volume.wav` при каких-то условиях. Хотя есть сомнения по этому поводу. Возможно там всё-таки производится инициализация движка медиаплеера. Может быть это что-то типа звукового оповещания при изменении уровней громкости?

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
+ const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 0, 0);
```

```diff
- const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 0, 1);
+ const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 1);
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

Пример использования похожих функций, которые удалось нагуглить: [concept.drm.txt](https://github.com/rajdeokumarsingh/Notes/blob/5f8089970773b9ae9c33f2feafe9bf4e1363bc9c/computer.science/android/multimedia/drm/drm.pekall.2.2/concept.drm.txt) и [flow.play.cpp](https://github.com/rajdeokumarsingh/Notes/blob/5f8089970773b9ae9c33f2feafe9bf4e1363bc9c/computer.science/android/multimedia/drm/drm.pekall.2.2/flow/flow.play.cpp), но похоже это не имеет ничего общего с реальностью.

Похоже что в версии от AURA и ZN5 библиотека `libezx_mp_playerengine.so.1` и `libmfsvc.so.1` для снятия DRM-защиты с файла используют функции с префиксом `WMDRM_`, которые в строках отладки отмечены как "Janus DRM Calls", одноимённая библиотека где-то валяется рядом. Да, обе этих библиотеки зависят от `libjanus.so.1`, вот таблица экспортируемых функций этой библиотеки, которые используются везде:

```bash
WMDRM_UTIL_CanBeAutoPlayed		 // FUNC
WMDRM_UTIL_DeleteLicense		 // FUNC
WMDRM_UTIL_GetLicenseDetail		 // FUNC
WMDRM_UTIL_IsDRMFile			 // FUNC
WMDRM_UTIL_LocateDRMObject		 // FUNC
WMDRM_UTIL_ValidateRights		 // FUNC
```

Flow для `libmfsvc.so.1`, в которой похоже содержатся всякие там виджет picker'ов различных медиафайлов:

```
DRM_GetContentMetaDataInFile() => DRM_CanSetForAutoUse() => DRM_CanForward()
DRM_GetContentMetaDataInFile() => DRM_HasUnlimitedRights()
DRM_IsDRMFile() =>  WMDRM_UTIL_IsDRMFile() => WMDRM_UTIL_CanBeAutoPlayed()
DRM_IsDRMFile() =>  WMDRM_UTIL_IsDRMFile() => DRM_GetContentMetaDataInFile() => WMDRM_UTIL_ValidateRights()
```

Flow для `libezx_mp_playerengine.so.1`, движка Media Player:

```
WMDRM_UTIL_IsDRMFile() => WMDRM_MGR_CanSeekByFile()
WMDRM_UTIL_IsDRMFile() => WMDRM_MGR_GetLicensInfo()
```

Это вот эта вот штука получается? https://en.wikipedia.org/wiki/Janus_(DRM) Похоже она не только для WMA файлов задействована? Или всё-таки только для них? В коде Media Player есть проверка на форматы WMA/WMV/ASF, похоже Janus используется только для них?

Нужно тщательнее снять логи работы `setupprofile` на AURA в следующий раз. Или же лучшим вариантом будет подготовка простенького приложения, которое использует движок плеера? По сути ему можно скормить путь к DRM-файлику и он его расшифрует. Нужно только разобраться как создавать объект класса `MediaPlayerEngine` и вызывать у него метод `open()`. Нужно ли потом делать какой-нибудь `play()`?

Может проще действительно использовать плеер и потом поиск в `/mnt/drmfs`? Есть ли туда доступ?
