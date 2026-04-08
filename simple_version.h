const int nmax = 256;
const float r2max = 5.0f;

void DrawMandelbrot(HDC *dc, COLORREF buf[600][800], double *x_start, double *y_start, double *zoom);
void ReallyDraw(HDC *dc, COLORREF buf[600][800]);
