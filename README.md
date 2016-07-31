# alber_rtsp_server

* 개발 환경
 * Windows 7 Ultimate K
 * Visual Studio 2008
 
* 빌드 방법
 * rtsp_server.sln 빌드
 * Output 파일 위치 : \\build\\bin\\dbg 또는 rel
 
* 실행 방법
 * rtsp_server.exe 실행
   * UDP Streaming이므로 방화벽 해제 
 * vlc와 같은 Client 프로그램에서 rtsp_server에 접속
   * mrl : rtsp://rtsp_server_pc_ip/alber_mjpeg
