# alber_rtsp_server

* 개발 환경
 * Windows 7 Ultimate K
 * Visual Studio 2008 / 2015
 
* 빌드 방법
 * rtsp_server_vcxx.sln 빌드
 * Output 파일 위치 : \\build\\bin\\vcxx\\dbg 또는 rel
 
* 실행 방법
 * rtsp_server_vcxx.exe 실행
   * UDP Streaming이므로 방화벽 해제 
 * vlc와 같은 Client 프로그램에서 rtsp_server에 접속
   * mrl : rtsp://rtsp_server_pc_ip/sample_file_name
     * sample_file_name 목록
        * mjpeg_test.avi
        * test_1.264
        * test_2.264
        * test_3.264
        * test_4.264
      * ex ) rtsp://127.0.0.1/mjpeg_test.avi
