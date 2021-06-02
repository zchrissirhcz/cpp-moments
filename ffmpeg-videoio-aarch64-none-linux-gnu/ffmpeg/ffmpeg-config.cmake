set(ffmpeg_path "${CMAKE_CURRENT_LIST_DIR}")

set(FFMPEG_EXEC_DIR "${ffmpeg_path}/bin")
set(FFMPEG_LIBDIR "${ffmpeg_path}/lib")
set(FFMPEG_INCLUDE_DIRS "${ffmpeg_path}/include")

# library names
set(FFMPEG_LIBRARIES
    ${FFMPEG_LIBDIR}/libavformat.a
    ${FFMPEG_LIBDIR}/libavdevice.a
    ${FFMPEG_LIBDIR}/libavcodec.a
    ${FFMPEG_LIBDIR}/libavutil.a
    ${FFMPEG_LIBDIR}/libswscale.a
    ${FFMPEG_LIBDIR}/libswresample.a
    ${FFMPEG_LIBDIR}/libavfilter.a
    ${FFMPEG_LIBDIR}/libavresample.a
    pthread
)

# found status
set(FFMPEG_libavformat_FOUND TRUE)
set(FFMPEG_libavdevice_FOUND TRUE)
set(FFMPEG_libavcodec_FOUND TRUE)
set(FFMPEG_libavutil_FOUND TRUE)
set(FFMPEG_libswscale_FOUND TRUE)
set(FFMPEG_libswresample_FOUND TRUE)
set(FFMPEG_libavfilter_FOUND TRUE)
set(FFMPEG_libavresample_FOUND TRUE)

# library versions
set(FFMPEG_libavformat_VERSION 58.71.100)
set(FFMPEG_libavdevice_VERSION 58.12.100)
set(FFMPEG_libavcodec_VERSION 58.129.100)
set(FFMPEG_libavutil_VERSION 56.67.100)
set(FFMPEG_libswscale_VERSION 5.8.100)
set(FFMPEG_libswresample_VERSION 3.8.100)
set(FFMPEG_libavfilter_VERSION 7.109.100)
set(FFMPEG_libavresample_VERSION 4.0.0)

set(FFMPEG_FOUND TRUE)
set(FFMPEG_LIBS ${FFMPEG_LIBRARIES})
