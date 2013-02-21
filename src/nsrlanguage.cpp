#include "nsrlanguage.h"

#include <QObject>
#include <QLocale>

static NSRLanguageData _languages[] = {
{
	/* Main window */
	QObject::trUtf8 ("Info"),
	QObject::trUtf8 ("Couldn't open document! "
			 "Seems that document has unsupported encryption version."),
	QObject::trUtf8 ("Couldn't open document!"),
	QObject::trUtf8 ("Open file"),
	QObject::trUtf8 ("All supported files"),
	QObject::trUtf8 ("PDF files"),
	QObject::trUtf8 ("DjVu files"),
	QObject::trUtf8 ("Text files"),
	QObject::trUtf8 ("TIFF files"),
	QObject::trUtf8 ("You are using text mode the first time. Note that document formatting "
			 "may be differ than in original one, no images displayed and page can "
			 "be empty if there is no text in the document. Also text may not be "
			 "displayed properly if appropriate language is not supported by phone."),
	QObject::trUtf8 ("You are using the Lite version of NSR Reader and it's restricted "
			 "only for %1 pages per document. If you want to view full document "
			 "without any restrictions please consider purchasing full version "
			 "at Nokia store."),
	QObject::trUtf8 ("Zoom to %:"),
	QObject::trUtf8 ("Page number (%1):"),
	QObject::trUtf8 ("File is already opened!"),
	QObject::trUtf8 ("No text data available for this page"),
	QObject::trUtf8 ("Enter password:"),
	QObject::trUtf8 ("Cancel"),
	/* Popup tool frame */
	QObject::trUtf8 ("Preferences"),
	QObject::trUtf8 ("Go to page"),
	QObject::trUtf8 ("About"),
	QObject::trUtf8 ("Exit"),
	/* Top left context menu */
	QObject::trUtf8 ("Fit to Width"),
	QObject::trUtf8 ("Zoom to %"),
	QObject::trUtf8 ("Rotate Left"),
	QObject::trUtf8 ("Rotate Right"),
	/* About window */
	QObject::trUtf8 ("Publisher:"),
	QObject::trUtf8 ("Ekimov Alexander Vladimirovich"),
	QObject::trUtf8 ("Developer:"),
	QObject::trUtf8 ("Alexander Saprykin,\n(c) 2011-2012"),
	QObject::trUtf8 ("Contacts:"),
	QObject::trUtf8 ("Used icons:"),
	QObject::trUtf8 ("If you liked NSR Reader, please leave a feedback  "
			 "in Nokia store on application's page."),
	QObject::trUtf8 ("Find on Facebook!"),
	QObject::trUtf8 ("You're using the Lite version. Buy full version in "
			 "Nokia store to view documents without any restrictions."),
        /* Splash screen */
	QObject::trUtf8 ("Loading..."),
	/* News window */
	QObject::trUtf8 ("What's new"),
	QObject::trUtf8 ("Welcome to"),
	QObject::trUtf8 ("New in this version:"),
	QObject::trUtf8 ("● first version of NSR Reader for MeeGo - have a great fun!"),
	/* Preferences window */
	QObject::trUtf8 ("Save last document's position"),
	QObject::trUtf8 ("Fullscreen mode"),
	QObject::trUtf8 ("Use text mode (column view)"),
	QObject::trUtf8 ("Inverted colors"),
	QObject::trUtf8 ("Text mode font:"),
	QObject::trUtf8 ("Text encoding:"),
	QObject::trUtf8 ("Select font"),
	QObject::trUtf8 ("Select encoding"),
	/* Touch dialog */
	QObject::trUtf8 ("Close"),
	/* Start label */
	QObject::trUtf8 ("Tips & Tricks"),
	QObject::trUtf8 ("Use pinch zoom and Volume Up/Down keys to navigate through pages"),
	QObject::trUtf8 ("Double tap on right or left side of the screen to go next or previous page"),
	QObject::trUtf8 ("Double tap center of the screen to fit page to width"),
	QObject::trUtf8 ("Continue to scroll page on the edge to go next or previous")
},
{
	/* Main window */
	QObject::trUtf8 ("Инфо"),
	QObject::trUtf8 ("Не удалось открыть документ. Возможно, алгоритм шифрования не поддерживается."),
	QObject::trUtf8 ("Не удалось открыть документ."),
	QObject::trUtf8 ("Открыть файл"),
	QObject::trUtf8 ("Все поддерживаемые"),
	QObject::trUtf8 ("Файлы PDF"),
	QObject::trUtf8 ("Файлы DjVu"),
	QObject::trUtf8 ("Текстовые файлы"),
	QObject::trUtf8 ("Файлы TIFF"),
	QObject::trUtf8 ("Вы используете текстовый режим первый раз. Форматирование может "
			 "отличаться от оригинала, изображения не будут отображаться, и страница "
			 "может быть пустой, если документ не содержит текста. Также текст может не "
			 "отображаться корректно, если соответствующий язык не поддерживается телефоном."),
	QObject::trUtf8 ("Вы используете Lite-версию NSR Reader, а она ограничена "
			 "только на первые %1 страниц документа. Если вы хотите просматривать весь "
			 "документ без ограничений, то, пожалуйста, купите полную версию в магазине Nokia."),
	QObject::trUtf8 ("Масштаб до %:"),
	QObject::trUtf8 ("Номер страницы (%1):"),
	QObject::trUtf8 ("Файл уже открыт!"),
	QObject::trUtf8 ("Данная страница не содержит текстовой информации"),
	QObject::trUtf8 ("Введите пароль:"),
	QObject::trUtf8 ("Отменить"),
	/* Popup tool frame */
	QObject::trUtf8 ("Параметры"),
	QObject::trUtf8 ("На страницу"),
	QObject::trUtf8 ("О программе"),
	QObject::trUtf8 ("Выход"),
	/* Top left context menu */
	QObject::trUtf8 ("По ширине"),
	QObject::trUtf8 ("Масштаб до %"),
	QObject::trUtf8 ("Повернуть влево"),
	QObject::trUtf8 ("Повернуть вправо"),
	/* About window */
	QObject::trUtf8 ("Издатель:"),
	QObject::trUtf8 ("Екимов Александр Владимирович"),
	QObject::trUtf8 ("Разработчик:"),
	QObject::trUtf8 ("Сапрыкин Александр,\n(c) 2011-2012"),
	QObject::trUtf8 ("Контакты:"),
	QObject::trUtf8 ("Иконки:"),
	QObject::trUtf8 ("Если вам понравилась NSR Reader - пожалуйста, оставьте "
			 "свой отзыв на странице приложения в магазине Nokia."),
	QObject::trUtf8 ("Ищите на Facebook!"),
	QObject::trUtf8 ("Вы используете Lite-версию. Купите полную версию в "
			 "магазине Nokia для просмотра документов без ограничений."),
	/* Splash screen */
	QObject::trUtf8 ("Загрузка..."),
	/* News window */
	QObject::trUtf8 ("Что нового"),
	QObject::trUtf8 ("Добро пожаловать в"),
	QObject::trUtf8 ("Новое в данной версии:"),
	QObject::trUtf8 ("● первая версия NSR Reader под MeeGo - удачного использования!"),
	/* Preferences window */
	QObject::trUtf8 ("Сохранять последнюю позицию"),
	QObject::trUtf8 ("Полноэкранный режим"),
	QObject::trUtf8 ("Текстовый (колоночный) режим"),
	QObject::trUtf8 ("Инвертировать цвета"),
	QObject::trUtf8 ("Шрифт текстового режима:"),
	QObject::trUtf8 ("Кодировка текста:"),
	QObject::trUtf8 ("Выбрать шрифт"),
	QObject::trUtf8 ("Выбрать кодировку"),
	/* Touch dialog */
	QObject::trUtf8 ("Закрыть"),
	/* Start label */
	QObject::trUtf8 ("Подсказки"),
	QObject::trUtf8 ("Масштабируйте мультитачем и используйте кнопки громкости для навигации"),
	QObject::trUtf8 ("Двойное касание правой или левой части экрана переводит на следующую или предыдущую страницы"),
	QObject::trUtf8 ("Двойное касание центра экрана масштабирует по ширине"),
	QObject::trUtf8 ("Прокрутка на краях экрана переводит на следующую или предыдущую страницы")
},
{
	/* Main window */
	QObject::trUtf8 ("जानकारी"),
	QObject::trUtf8 ("दस्तावेज खोला नहीं जा सका! ऐसा प्रतीत होता है कि दस्तावेज असमर्थित एनक्रिप्टयुक्त वर्शन में था।"),
	QObject::trUtf8 ("दस्तावेज नहीं खोला जा सका!"),
	QObject::trUtf8 ("फाइल खोलें"),
	QObject::trUtf8 ("सभी समर्थित फाइलें"),
	QObject::trUtf8 ("PDF फाइलें"),
	QObject::trUtf8 ("DjVu फाइलें"),
	QObject::trUtf8 ("टेक्स्ट फाइलें"),
	QObject::trUtf8 ("TIFF फाइलें"),
	QObject::trUtf8 ("आप टेक्स्ट मोड का उपयोग पहली बार कर रहे हैं। कृपया नोट करें कि दस्तावेज की फॉर्मेटिंग मूल से अलग "
			 "हो सकती है, अगर दस्तावेज में कोई पाठ नहीं है तो कोई भी प्रदर्शित चित्र और पेज खाली नहीं हो सकते हैं। "
			 "साथ ही, अगर फोन के द्वारा उचित भाषा समर्थित नहीं है तो पाठ ठीक से प्रदर्शित नहीं हो सकता है।"),
	QObject::trUtf8 ("आप NSR Reader के Lite वर्शन का उपयोग कर रहे हैं और यह प्रति दस्तावेज केवल %1 पेज के लिए "
			 "सीमित है। अगर आप बिना किसी प्रतिबंध के पूरा दस्तावेज देखना चाहते हैं तो कृपया Nokia स्टोर में फुल वर्शन खरीदें।"),
	QObject::trUtf8 ("% ज़ूम करें:"),
	QObject::trUtf8 ("पेज संख्या (%1):"),
	QObject::trUtf8 ("फाइल पहले से ही खुली है!"),
	QObject::trUtf8 ("इस पेज के लिए टेक्स्ट डाटा उपलब्ध नहीं है"),
	QObject::trUtf8 ("पासवर्ड दर्ज करें:"),
	QObject::trUtf8 ("रद्द करना"),
	/* Popup tool frame */
	QObject::trUtf8 ("प्राथमिकताएं"),
	QObject::trUtf8 ("पेज पर जाएं"),
	QObject::trUtf8 ("के बारे में"),
	QObject::trUtf8 ("निकास"),
	/* Top left context menu */
	QObject::trUtf8 ("चौड़ाई  के लिए फिट"),
	QObject::trUtf8 ("% ज़ूम करें"),
	QObject::trUtf8 ("बायें घुमाएं"),
	QObject::trUtf8 ("दायें घुमाएं"),
	/* About window */
	QObject::trUtf8 ("पब्लिशर:"),
	QObject::trUtf8 ("Ekimov Alexander Vladimirovich"),
	QObject::trUtf8 ("डेवलपर:"),
	QObject::trUtf8 ("Alexander Saprykin,\n(c) 2011-2012"),
	QObject::trUtf8 ("संपर्क:"),
	QObject::trUtf8 ("प्रयुक्त आइकन:"),
	QObject::trUtf8 ("अगर NSR Reader आपको पसंद आया हो तो कृपया अनुप्रयोग पेज पर Nokia स्टोर में फीडबैक दें।"),
	QObject::trUtf8 ("Facebook पर पाएं!"),
	QObject::trUtf8 ("आप Lite वर्शन का उपयोग कर रहे हैं। दस्तावेजों को बिना किसी प्रतिबंध के देखने के लिए Nokia स्टोर में फुल वर्शन खरीदें।"),
	/* Splash screen */
	QObject::trUtf8 ("लोड हो रहा है..."),
	/* News window */
	QObject::trUtf8 ("नया क्या है"),
	QObject::trUtf8 ("स्वागत"),
	QObject::trUtf8 ("इस वर्शन में नया:"),
	QObject::trUtf8 ("● first version of NSR Reader for MeeGo - have a great fun!"),
	/* Preferences window */
	QObject::trUtf8 ("पिछले दस्तावेज की स्थिति सहेजें"),
	QObject::trUtf8 ("फुल स्क्रीन मोड"),
	QObject::trUtf8 ("टेक्स्ट मोड (कॉलम व्यू) का उपयोग करें"),
	QObject::trUtf8 ("इनवर्टेड कलर्स"),
	QObject::trUtf8 ("टेक्स्ट मोड फांट:"),
	QObject::trUtf8 ("टेक्स्ट एनकोडिंग:"),
	QObject::trUtf8 ("फांट चुनें"),
	QObject::trUtf8 ("एनकोडिंग चुनें"),
	/* Touch dialog */
	QObject::trUtf8 ("बंद करें"),
	/* Start label */
	QObject::trUtf8 ("टिप्स और ट्रिक्स"),
	QObject::trUtf8 ("पेजों पर मार्गदर्शन के लिए ज़ूम और वॉल्यूम अप/डाउन कुंजी का उपयोग करें"),
	QObject::trUtf8 ("अगले या पिछले पेज पर जाने के लिए स्क्रीन के दायीं या बायीं तरफ डबल टैप करें"),
	QObject::trUtf8 ("पेज को चौड़ाई के अनुसार फिट करने के लिए स्क्रीन के बीच में डबल टैप करें"),
	QObject::trUtf8 ("आगे या पीछे जाने के लिए पेज को किनारों पर स्क्रॉल करना जारी रखें")
},
{
	/* Main window */
	QObject::trUtf8 ("Info"),
	QObject::trUtf8 ("Non posso aprire il documento! Sembrerebbe avere una versione di crittografia non supportata."),
	QObject::trUtf8 ("Non posso aprire il documento!"),
	QObject::trUtf8 ("Apri il file"),
	QObject::trUtf8 ("Tutti i file supportati"),
	QObject::trUtf8 ("Files PDF"),
	QObject::trUtf8 ("Files DjVu"),
	QObject::trUtf8 ("Files di testo"),
	QObject::trUtf8 ("Files TIFF"),
	QObject::trUtf8 ("Stai usando la versione testo per la prima volta. Considera che "
			 "la formattazione del documento potrebbe essere diversa dall'originale, "
			 "senza immagini (quindi, se non c'è testo, potresti vedere solo un file "
			 "vuoto). Lo stesso testo potrebbe essere visualizzato male se la lingua "
			 "corretta non è supportata dal telefono."),
	QObject::trUtf8 ("Stai usando la versione Lite dell'NSR Reader, che è limitata a %1 pagine "
			 "per documento. Se vuoi vedere i documenti per intero senza restrizioni, "
			 "l'acquisto è disponibile sul Nokia store."),
	QObject::trUtf8 ("Zoom al %:"),
	QObject::trUtf8 ("Numero di pagina (%1):"),
	QObject::trUtf8 ("Il file è già aperto!"),
	QObject::trUtf8 ("Nessun dato disponibile per questa pagina"),
	QObject::trUtf8 ("Inserisci la password:"),
	QObject::trUtf8 ("Cancellare"),
	/* Popup tool frame */
	QObject::trUtf8 ("Preferenze"),
	QObject::trUtf8 ("Vai alla pagina"),
	QObject::trUtf8 ("Crediti"),
	QObject::trUtf8 ("Uscita"),
	/* Top left context menu */
	QObject::trUtf8 ("Riduci alla larghezza"),
	QObject::trUtf8 ("Zoom al %"),
	QObject::trUtf8 ("Ruota a sinistra"),
	QObject::trUtf8 ("Ruota a destra"),
	/* About window */
	QObject::trUtf8 ("Editore:"),
	QObject::trUtf8 ("Ekimov Alexander Vladimirovich"),
	QObject::trUtf8 ("Sviluppatore:"),
	QObject::trUtf8 ("Alexander Saprykin,\n(c) 2011-2012"),
	QObject::trUtf8 ("Contatti:"),
	QObject::trUtf8 ("Icone utilizzate:"),
	QObject::trUtf8 ("Se ti piace l'NSR Reader, si prega di lasciare un feedback "
			 "sulla pagina dell'applicazione nel Nokia store."),
	QObject::trUtf8 ("Trova su Facebook!"),
	QObject::trUtf8 ("Stai usando la versione Lite. Compra la versione completa "
			 "sul Nokia store per vedere i documenti senza restrizioni."),
	/* Splash screen */
	QObject::trUtf8 ("In caricamento..."),
	/* News window */
	QObject::trUtf8 ("Novità..."),
	QObject::trUtf8 ("Benvenuto su"),
	QObject::trUtf8 ("Novità di questa versione:"),
	QObject::trUtf8 ("● first version of NSR Reader for MeeGo - have a great fun!"),
	/* Preferences window */
	QObject::trUtf8 ("Salva posizione ultimo documento"),
	QObject::trUtf8 ("Fullscreen"),
	QObject::trUtf8 ("Modalità testo (vista a colonne)"),
	QObject::trUtf8 ("Colori invertiti"),
	QObject::trUtf8 ("Font in modalità testo:"),
	QObject::trUtf8 ("Codifica del testo:"),
	QObject::trUtf8 ("Scegli il font"),
	QObject::trUtf8 ("Scegli la codifica"),
	/* Touch dialog */
	QObject::trUtf8 ("Chiudi"),
	/* Start label */
	QObject::trUtf8 ("Trucchi"),
	QObject::trUtf8 ("Usa il pinch zoom e i tasti volume su/giù per navigare"),
	QObject::trUtf8 ("Doppio tap sul lato destro o sinistro dello schermo per andare alla prossima/precedente pagina"),
	QObject::trUtf8 ("Doppio tap al centro dello schermo per adattare la larghezza della pagina allo schermo"),
	QObject::trUtf8 ("Continua a scorrere la pagina al bordo per andare alla prossima o alla precedente")
}
};

NSRLanguage::NSRLanguage ()
{
	QLocale locale = QLocale::system ();

	if (locale.language () == QLocale::Italian)
		_langIndex = 3;
	else if (locale.language () == QLocale::Hindi)
		_langIndex = 2;
	else if (locale.language () == QLocale::Russian)
		_langIndex = 1;
	else
		_langIndex = 0;
}

NSRLanguage *NSRLanguage::instance()
{
	static NSRLanguage *_instance;

	if (_instance == NULL)
		_instance = new NSRLanguage ();

	return _instance;
}

const NSRLanguageData *NSRLanguage::getLanguage()
{
	return (_languages + _langIndex);
}
