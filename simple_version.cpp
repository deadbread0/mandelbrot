// #define TX_USE_SFML
#include "TX\TXLib.h" 
#include <stdio.h>
#include <windows.h>
#include "simple_version.h"

int main()
{
    HWND wtf = txCreateWindow (800, 600);
    // HDC dc = txDC();
    // POINT size = txGetExtent();
    double size_x = 800, size_y = 600;

    RGBQUAD* buf = NULL;                                      // Do NOT actually create the array!
    HDC dc = txCreateDIBSection(size_x, size_y, &buf);
    COLORREF buff [600][800] = {};                                      // Do NOT actually create the array!
    
    double x_start = 0, y_start = 0, zoom = 1;
    
    // DrawMandelbrot(&dc, &buf, &x_start, &y_start);
    
     while (!GetAsyncKeyState (VK_ESCAPE))
                  {
                    DrawMandelbrot(&dc, buff, &x_start, &y_start, &zoom); // fake draw
                    ReallyDraw(&dc, buff);
                    txBitBlt (txDC(), 0, 0, size_x, size_y, dc, 0, 0);
                    if (txGetAsyncKeyState (VK_RIGHT)) x_start -= 15;
                    if (txGetAsyncKeyState (VK_LEFT)) x_start += 15;
                    if (txGetAsyncKeyState (VK_UP)) y_start += 15;
                    if (txGetAsyncKeyState (VK_DOWN)) y_start -= 15;
                    if (txGetAsyncKeyState (VK_OEM_PLUS)) zoom *= 1.005;
                    if (txGetAsyncKeyState (VK_OEM_MINUS)) zoom *= 0.995;
                    txSleep (0);
                  }

    txDeleteDC (dc);
    ReleaseDC (wtf, dc);

    return 0;
}

void DrawMandelbrot(HDC *dc, COLORREF buf[600][800], double *x_start, double *y_start, double *zoom)
{
    SYSTEMTIME st;

    GetLocalTime(&st);
    // int skip_sec_start = 0;
    int skip_msec_start = 0;

    float xc = ( 400.f + ( float(*x_start) - 300.f) * 0.01f / float(*zoom) );
    float yc = ( float(*y_start) - 300.f ) * 0.01f / float(*zoom) + 300.f; 

    for (int iy = 0; iy < 600; iy++)
    {
        // int sec_start = st.wSecond;
        // int msec_start = st.wMilliseconds + (st.wSecond + st.wMinute) * 1000;
        // printf ("FPS %d\t\t\r", (msec_start + sec_start * 1000));


        float dx = 0.001f / float(*zoom), 
              dy = 0.001f / float(*zoom);
              
        float x0 = (( float(*x_start) - 300.f) * dx);
        float y0 = ((float)iy + float(*y_start) - 300.f) * dy;

        // float xc = ( 400.f + x0 );
        // float yc = ( float(*y_start) - 300.f ) * dy + 300.f;
        // printf ("FPS %lf %lf\t\t\r", y0, yc); 

        // x0 = xc + (x0 - xc) / float(*zoom);
        // y0 = yc + (y0 - yc) / float(*zoom);

        for (int ix = 0; ix < 800; ix++, x0 += dx)
        {
            float x = x0,
                  y = y0;

            // x = (x - xc) / float(*zoom) + xc;
            // y = (y - yc) / float(*zoom) + yc;

            int n = 0;

            for (; n < nmax; n++)
            {
                float x2 = x * x,
                      y2 = y * y,
                      xy = x * y;

                float r2 = x2 + y2;

                if (r2 >= r2max) break;

                x = x2 - y2 + x0;
                y = xy + xy + y0;

                // x = (x - xc) / float(*zoom) + xc;
                // y = (y - yc) / float(*zoom) + yc;
            }

            // skip_sec_start += (st.wSecond - sec_start); //время, за которое выполняется 1 итерация по у и все итерации одного цикла по х
            // skip_msec_start += (st.wMilliseconds - msec_start);
            // printf ("FPS %d %d\t\t\r", (msec_start + sec_start * 1000), st.wSecond * 1000 + st.wMilliseconds);


            double i = sin(iy) * 50 + ix;
            COLORREF color = (n < nmax)? COLORREF RGB(128 - i, i, 255 - i) : COLORREF RGB(0, 0, 0);
            // RGBQUAD* color     =  buf [ix + iy * 800];
            // RGBQUAD colorr = (n < nmax)? RGBQUAD {(BYTE) (128 - i), (BYTE) i, (BYTE) (255 - i)} : RGBQUAD {0, 0, 0};
            buf[iy][ix] = color;
        }
    }

    printf ("FPS %f\t\t\r", txGetFPS());
}

void ReallyDraw(HDC *dc, COLORREF buf[600][800])
{
    for (int iy = 0; iy < 600; iy++)
    {
        for (int ix = 0; ix < 800; ix++)
        {
            txSetPixel(ix, iy, buf[iy][ix], *dc);
        }
    }
}
