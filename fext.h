#ifndef FEXT_H
#define FEXT_H

typedef enum ext {
    EXT_UNKNOWN = 0,

    /* text / data */
    EXT_TXT,
    EXT_CSV,
    EXT_JSON,
    EXT_XML,
    EXT_YAML,

    /* source code */
    EXT_C,
    EXT_H,
    EXT_CPP,
    EXT_HPP,
    EXT_PY,
    EXT_JS,
    EXT_TS,
    EXT_JAVA,
    EXT_RS,
    EXT_GO,

    /* scripting */
    EXT_SH,
    EXT_BAT,

    /* binaries */
    EXT_EXE,
    EXT_BIN,
    EXT_SO,
    EXT_DLL,
    EXT_A,

    /* images */
    EXT_PNG,
    EXT_JPG,
    EXT_JPEG,
    EXT_GIF,
    EXT_BMP,
    EXT_SVG,
    EXT_WEBP,

    /* audio */
    EXT_MP3,
    EXT_WAV,
    EXT_FLAC,
    EXT_OGG,

    /* video */
    EXT_MP4,
    EXT_MKV,
    EXT_AVI,
    EXT_MOV,
    EXT_WEBM,

    /* archives */
    EXT_ZIP,
    EXT_TAR,
    EXT_GZ,
    EXT_7Z,
    EXT_RAR,

    /* documents */
    EXT_PDF,
    EXT_DOC,
    EXT_DOCX,
    EXT_XLS,
    EXT_XLSX,
    EXT_PPT,
    EXT_PPTX,

    EXT_COUNT   /* always last */
} ext;

/* string table */
static const char* ext_str[EXT_COUNT] = {
    "unknown",

    /* text / data */
    "txt",
    "csv",
    "json",
    "xml",
    "yaml",

    /* source code */
    "c",
    "h",
    "cpp",
    "hpp",
    "py",
    "js",
    "ts",
    "java",
    "rs",
    "go",

    /* scripting */
    "sh",
    "bat",

    /* binaries */
    "exe",
    "bin",
    "so",
    "dll",
    "a",

    /* images */
    "png",
    "jpg",
    "jpeg",
    "gif",
    "bmp",
    "svg",
    "webp",

    /* audio */
    "mp3",
    "wav",
    "flac",
    "ogg",

    /* video */
    "mp4",
    "mkv",
    "avi",
    "mov",
    "webm",

    /* archives */
    "zip",
    "tar",
    "gz",
    "7z",
    "rar",

    /* documents */
    "pdf",
    "doc",
    "docx",
    "xls",
    "xlsx",
    "ppt",
    "pptx"
};

#endif // FEXT_H

