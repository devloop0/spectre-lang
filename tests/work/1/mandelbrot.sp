import <"std/syscall">

func void putchar(char c) {
	std::syscall::direct_write(1, c$, 1);
}

func int main() 
{ 
    double x, y; 
    const int maxiter = 32; 
    int iter; 
 
    for (int b = 0; b <= 32; b++)
    { 
        for (int a = 0; a <= 85; a++)
        { 
            double zi = 0.0, zr = 0.0; 
            double zni = 0.0, znr = 0.0; 
            x = (a - 50) / 20.0;    
            y = (b - 16.0) / 10.0;  
            iter = 1; 
            zi = 0.0; zr = 0.0; 
            do { 
                zni = 2 * zi * zr + y;        
                znr = zr * zr - zi * zi + x;  
                zi = zni; 
                zr = znr; 
                if (zi*zi + zr*zr > 4.0) 
                    break; 
 
                iter++; 
            } while (iter < maxiter); 
            if (iter == maxiter) 
                putchar(' '); 
            else 
                putchar (" .:-;!/>)|&IH%*#"[iter & 15]); 
        } 
        putchar ('\n'); 
    } 
    return 0; 
}
