// -------------- ACCESS TO THE SENSOR DRIVERS -----------------

// global sensor constants and data

// port types for analog sensor
#define MODE_COLORFULL 13
#define MODE_COLORRED  14
#define MODE_COLORGREEN 15
#define MODE_COLORBLUE 16
#define MODE_COLORNONE 17

// sizes and offsets for shared memory areas
#define ANALOG_SIZE  5172
#define ANALOG_NXTCOL_OFF 4856
#define ANALOG_NXTCOL_SZ  72
#define ANALOG_NXTCOL_RAW_OFF 54
#define UART_SIZE   42744
#define UART_SET_CONN  0xc00c7500
#define DEV_ACTUAL_OFF  42592
#define DEV_RAW_SIZE1   9600
#define DEV_RAW_SIZE2  32
#define DEV_RAW_OFF 4192

// operation modes for EV3 color sensor
#define COL_RESET  -1
#define COL_REFLECT  0
#define COL_AMBIENT  1
#define COL_COLOR   2
#define COL_REFRAW  3
#define COL_RGBRAW   4
#define COL_CAL     5

// handles for devices and pointers to memory mapped regions
int dcmDevice = -1;
int analogDevice = -1;
byte* pAnalog = NULL;
int uartDevice = -1;
byte* pUART = NULL;

int uart_read_position[4];  // index to circular input buffer

// init/exit the sensors

int SensorInit()
{
    dcmDevice = open(LMS_DCM_DEVICE_NAME, O_RDWR);
    if (dcmDevice<0)
    {   SensorExit();
        return 0;
    }
    analogDevice = open(LMS_ANALOG_DEVICE_NAME, O_RDWR | O_SYNC);
    if (analogDevice<0)
    {   SensorExit();
        return 0;
    }
    byte* tmp = mmap(0, ANALOG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, analogDevice, 0);
    if (tmp == MAP_FAILED || tmp==NULL)
    {   SensorExit();
        return 0;
    }
    uartDevice = open(LMS_UART_DEVICE_NAME, O_RDWR | O_SYNC);
    if (uartDevice<0)
    {   SensorExit();
        return 0;
    }
    pAnalog = tmp;

    tmp = mmap(0, UART_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, uartDevice, 0);
    if (tmp == MAP_FAILED || tmp==NULL)
    {   SensorExit();
        return 0;
    }
    pUART = tmp;

    int i;
    for (i=0; i<4; i++)
    {   uart_read_position[i] = 0;
    }

    return 1;
}

int SensorExit()
{
    if (pUART!=NULL)
    {   munmap(pUART, UART_SIZE);
        pAnalog=NULL;
    }
    if (uartDevice>=0)
    {   close(uartDevice);
        uartDevice=-1;
    }
    if (pAnalog!=NULL)
    {   munmap(pAnalog, ANALOG_SIZE);
        pAnalog=NULL;
    }
    if (analogDevice>=0)
    {   close(analogDevice);
        analogDevice=-1;
    }
    if (dcmDevice>=0)
    {    close(dcmDevice);
         dcmDevice=-1;
    }
}

// change operation mode of sensor port
// normally not necessary when auto-detection. used specifically to
// set mode of nxt color sensor
int SensorPortMode(int port, int mode)
{
    byte modes[4];
    modes[0] = '-';
    modes[1] = '-';
    modes[2] = '-';
    modes[3] = '-';
    modes[port] = mode;
    if (write(dcmDevice,modes,4) != 4)
    {   return 0;
    }
    return 1;
}

// read out values of NXT color sensor
void SensorReadNXTRGB(int port, int raw[])
{
    int i;
    for (i=0; i<4; i++)
    {   int off =  ANALOG_NXTCOL_OFF + port*ANALOG_NXTCOL_SZ + ANALOG_NXTCOL_RAW_OFF + i*2;
        raw[i] = *((short*)(pAnalog + off));
    }
}

void SensorSetUARTOperatingModes(int mode0, int mode1, int mode2,  int mode3)
{
    byte devcon[3*NUM_INPUTS];

    devcon[0] = CONN_INPUT_UART;
    devcon[0+NUM_INPUTS] = 0;
    devcon[0+2*NUM_INPUTS] = (byte)mode0;

    devcon[1] = CONN_INPUT_UART;
    devcon[1+NUM_INPUTS] = 0;
    devcon[1+2*NUM_INPUTS] = (byte)mode1;

    devcon[2] = CONN_INPUT_UART;
    devcon[2+NUM_INPUTS] = 0;
    devcon[2+2*NUM_INPUTS] = (byte)mode2;

    devcon[3] = CONN_INPUT_UART;
    devcon[3+NUM_INPUTS] = 0;
    devcon[3+2*NUM_INPUTS] = (byte)mode3;

    ioctl(uartDevice, UART_SET_CONN, devcon);
}

// Empties the UART input buffer for the given port.
// When new data arrives, the buffer gets filled up and can be read again.
void SensorUARTDiscardInputBuffer(int port)
{
    // determine what is the newest slot in the circular input buffer
    int actual = *((short*)(pUART+DEV_ACTUAL_OFF+port*2));

    // set this as new read position
    uart_read_position[port] = actual;
}

// Read next data sample from the UART input buffer.
// Return 1 if a sample was available, 0 otherwise
int SensorUARTGetShorts(int port, short* buffer, int len)
{
    // determine what is the newest slot in the circular input buffer
    int actual = *((short*)(pUART+DEV_ACTUAL_OFF+port*2));

    // check for misbehaviour of the UART driver
    if (actual<0 || actual>=300)
    {   return 0;
    }
    // check if any data has arrived yet
    if (uart_read_position[port]==actual)
    {   return 0;
    }
    // increment read_position towards the new actual pointer
    int index = (uart_read_position[port]+1)%300;
    uart_read_position[port] = index;

    // fetch data from the correct place
    short* source = (short*) (pUART + DEV_RAW_OFF + (port*DEV_RAW_SIZE1 + index*DEV_RAW_SIZE2));
    int i;
    for (i=0; i<len; i++)
    {   buffer[i] = source[i];
    }
    // return 1 to signal successful read
    return 1;
}

int SensorUARTGetActual(int port)
{
    int actual = *((short*)(pUART+DEV_ACTUAL_OFF+port*2));
    return actual;
}

