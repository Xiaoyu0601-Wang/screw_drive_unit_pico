#ifndef _COMM_H_
#define _COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DEV_Config.h"
// #include "buffer.h"
// #include "stm32_time.h"

/**
 * \brief Possible communication states
 *
 */
typedef enum {
  COMM_SUCCESS = 0x00, /*!< The communication module has ended successfully */
  COMM_BUSY = 0x01,    /*!< The communication module is busy */
  COMM_ERROR = 0x02,   /*!< The communication module has ended with an error */
  COMM_TIMEOUT = 0x03, /*!< The communication module waited too long for data */
  COMM_FULL = 0x04     /*!< The communication data buffer is full */
} comm_error;

/**
 * \brief Generic communication device structure
 *
 * This structure implements a generic interface to a communication device in
 * three different modes: blocking, interrupt driven and DMA driven.
 *
 * It is intended to be the intermediate structure between a user application
 * that requires access to a communication device, and the actual communication
 * device.
 *
 * On the communication device side, a set of functions must be implemented and
 * assigned to this structure at initialization time. Furthermore, the module
 * implementing this functions should implement the following features:
 *  * Configure the hardware to operate as desired.
 *  * Configure the interrupts and their priorities.
 *  * Configure the DMA transactions (in case they are enabled).
 *  * Implement the functions required by this structure to initiate and cancel
 *    RX and TX transactions using both IRQ and DMA.
 *  * Implement the basic IRQ and DMA handler functions.
 *
 * On the user application side, another set of functions must be also
 * implemented and assigned to this structure at initialization time. The user
 * application should be in charge of initializing this structure and any other
 * data structure required.
 *
 * In blocking mode, the IRQ driven transactions are used except when the use of
 * DMA is enabled, in which case the DMA driven transactions are used.
 *
 * The following diagram describes the transmission operation when using
 * interrupts, and how the user application, the communication module and the
 * actual communication device work together:
 *
 * ![](tx_irq_driven.png)
 *
 * The user application starts a IRQ driven transmission by calling the
 * comm_send_irq() function, which in turn calls the low level communication
 * function pointed by the parameter send_irq. At this point the interrupt
 * handler is called each time a byte is actually sent, which calls the
 * comm_do_irq_send() function to update the internal state and, in turn, calls
 * the user callback function pointed by irq_send_cb.
 *
 * While the data transfer is in progress the user can call the
 * comm_is_send_done() function to check the status of the transmission and the
 * comm_get_error() function to check for errors (if any). This functions can be
 * used with any kind of non-blocking transactions (IRQ and DMA).
 *
 * The following diagram describes the reception operation when using
 * interrupts, and how the user application, the communication module and the
 * actual communication device work together:
 *
 * ![](rx_irq_driven.png)
 *
 * The reception is only enabled when the user application calls the
 * comm_receive_irq() function, all data received before this function is called
 * will be lost. This function calls the low level communication function
 * pointed by the parameter receive_irq.
 *
 * At this point the interrupt handler is called each time a byte is received,
 * which calls the comm_do_irq_receive() function to update the internal state
 * and, in turn, calls the user callback function pointed by irq_receive_cb. The
 * reception remains active until the desired number of bytes are received, or
 * the user application cancels it.
 *
 * To get the received data, use the comm_get_received_data() function.
 *
 * The following diagram descrives the transmission operation when using
 * DMA, and how the user application, the communication module and the
 * actual communication device work together:
 *
 * ![](tx_dma_driven.png)
 *
 * The user application starts a DMA driven transmission by calling the
 * comm_send_dma() function, which in turn calls the low level communication
 * function pointed by the parameter send_dma. This function should configure
 * the DMA transfer and start it.
 *
 * When the last byte to be transmitted is written to the communication device,
 * the DMA interrupt handler is called, which calls the comm_do_dma_send()
 * function which enables the TX interrupts to wait until the last byte has
 * been actually sent.
 *
 * Finally, when the TX interrupt handler is called, the comm_do_irq_send()
 * function is called, in the same way it is called in a regular IRQ driven
 * transmission. In this case however, its behavior is different, because
 * the user application callback function pointed by dma_send_cb is called,
 * so that the user can perform any required action.
 *
 * The following diagram descrives the reception operation when using
 * DMA, and how the user application, the communication module and the
 * actual communication device work together:
 *
 * ![](rx_dma_driven.png)
 *
 * The user application starts a DMA driven reception by calling the
 * comm_receive_dma() function, which in turn calls the low level communication
 * function pointed by the parameter receive_dma. This function should configure
 * the DMA transfer and start it.
 *
 * When the desired number of bytes have been received, the reception DMA
 * interrupt handler is called, which calls the comm_do_dma_receive() function,
 * calls the user application function pointed by the dma_receive_cb parameter,
 * so that the user can perform any required action.
 *
 * In the case of DMA driven reception, the received data can be accessed by the
 * data pointer provided to the comm_receive_dma() function, once the reception
 * has ended.
 */
typedef struct {
  /* private data */
  /**
   * \brief Transmission data buffer
   *
   * This parameter holds the data to be transmitted until they can be actually
   * sent.
   */
  TBuffer tx_buffer;
  /**
   * \brief Active transmission using IRQ flag
   *
   * This parameter is used to indicate if there is an active transmission of
   * data using interrupts to handle it. This flag is set when a transmission
   * starts and it is cleared when it ends.
   */
  volatile unsigned char irq_sending;
  /**
   * \brief Active transmission using DMA flag
   *
   * This parameter is used to indicate if there is an active transmission of
   * data using DMA to handle it. This flag is set when a transmission starts
   * and it is cleared when it ends.
   */
  volatile unsigned char dma_sending;
  /**
   * \brief Number of bytes pending to be sent
   *
   * This parameter holds the number of bytes pending to be sent in the current
   * transaction. This parameter is initialized when the transmission starts,
   * and is decremented after the transmission of each byte.
   */
  unsigned short int tx_length;
  /**
   * \brief Reception data buffer
   *
   * This parameter holds the data to be transmitted until they can be actually
   * sent.
   */
  TBuffer rx_buffer;
  /**
   * \brief Active reception using IRQ flag
   *
   * This parameter is used to indicate if there is an active reception of data
   * using interrupts to handle it. This flag is set when a reception starts and
   * it is cleared when it ends.
   */
  volatile unsigned char irq_receiving;
  /**
   * \brief Active reception using DMA flag
   *
   * This parameter is used to indicate if there is an active reception of data
   * using DMA to handle it. This flag is set when a reception starts and it is
   * cleared when it ends.
   */
  volatile unsigned char dma_receiving;
  /**
   * \brief Number of bytes pending to be received
   *
   * This parameter holds the number of bytes pending to be received in the
   * current transaction. This parameter is initialized when the reception
   * starts, and is decremented after the reception of each byte.
   */
  unsigned short int rx_length;
  /**
   * \brief Use DMA flag
   *
   * This parameter indicates if the communication module uses DMA or not to
   * complete any transmission and reception.
   */
  unsigned char use_dma;
  /**
   * \brief DMA termination flag
   *
   * This parameter is used in the comm_do_irq_send() function to indicate
   * whether the interrupt is generated by the last transmitted byte of a DMA
   * transaction or not.
   */
  unsigned char dma_waiting_buffer_empty;
  /**
   * \brief Last communication error
   *
   * This parameter holds the error of the last communication transaction
   * (either transmission or reception).
   */
  unsigned char error;
  /**
   * \brief Time structure to handle timeouts
   *
   * This parameter is used to handle reception timeouts to avoid that the
   * reception functions get blocked due to missing bytes or communication
   * errors.
   */
  TTime *time;
  /**
   * \brief pointer to a user data structure
   *
   * This parameter is used to store a user data structure which is passed as a
   * parameter to the user callback functions (irq_send_cb(),irq_receive_cb(),
   * dma_send_cb() and dma_receive_cb()).
   */
  void *data;
  /* IRQ functions */
  /**
   * \defgroup hardware_irq_pointers Hardware IRQ related function pointers
   *
   * This set of functions, that must be implemented by the low level hardware
   * communication module (either USART, I2C, SPI or any other communication
   * device), are intended to handle interrupt driven receptions and
   * transmissions.
   *
   * This function pointers must be assigned by the initialization function
   * of the communication module.
   *
   */
  /**@{*/
  /**
   * \brief Pointer to a function to start a transmission using interrupts
   *
   * This function should send the first byte through the communication device,
   * clear any pending interrupt transmission complete flag and enable this
   * interrupt in order to send the remaining of the data.
   */
  unsigned char (*send_irq)(unsigned char first_byte);
  /**
   * \brief Pointer to a function to enable the transmission interrupts
   *
   * This function should clear any pending transmission complete interrupt and
   * enable this interrupt, but without sending any data through the
   * communication device.
   */
  unsigned char (*enable_tx_irq)(void);
  /**
   * \brief Pointer to a function to start a reception using interrupts
   *
   * This function should only enable the reception interrupts so that the IRQ
   * handler function is called each time a new data byte is received.
   */
  unsigned char (*receive_irq)(void);
  /**
   * \brief Pointer to a function to cancel a current reception using interrupts
   *
   * This function should only disable the reception interrupts so that the IRQ
   * handler function is no longer called.
   */
  unsigned char (*cancel_receive_irq)(void);
  /**@}*/
  /* DMA functions */
  /**
   * \defgroup hardware_dma_pointers Hardware DMA related function pointers
   *
   * This set of functions, that must be implemented by the low level hardware
   * communication module (either USART, I2C, SPI or any other communication
   * device), are intended to handle DMA driven receptions and transmissions.
   *
   * This function pointers must be assigned by the initialization function
   * of the communication module.
   *
   */
  /**@{*/
  /**
   * \brief Pointer to a function to start a transmission using DMA
   *
   * This function should configure and enable the corresponding DMA channel
   * to transfer the desired amount of data from the internal buffer to the
   * communication device. TX DMA interrupts should be enabled so that the
   * corresponding IRQ handler is called when the transaction is complete or
   * when an error occurs.
   */
  unsigned char (*send_dma)(unsigned char *data, unsigned short int length);
  /**
   * \brief Pointer to a function to start a reception using DMA
   *
   * This function should configure and enable the corresponding DMA channel
   * to transfer the desired amount of data from the communication device to
   * the internal buffer. RX DMA interrupts should be enabled so that the
   * corresponding IRQ handler is called when the transaction is complete or
   * when an error occurs.
   */
  unsigned char (*receive_dma)(unsigned char *data, unsigned short int length);
  /**
   * \brief Pointer to a function to cancel a current reception using DMA
   *
   * This function should abort the current RX DMA transaction in case there
   * is any active.
   */
  unsigned char (*cancel_receive_dma)(void);
  /**@}*/
  /* irq callbacks */
  /**
   * \defgroup callback_irq_pointers User IRQ related function pointers
   *
   * This set of functions, that must be implemented by the user module that
   * uses the communication structure, are intended to handle the specific
   * requirements of each application using interrupt driven communications.
   *
   * This function pointers must be assigned by the initialization function
   * of the user module.
   *
   */
  /**@{*/
  /**
   * \brief Callback function to handle the end of interrupt driven
   * transmissions
   *
   * This function is called whenever an IRQ driven transmission is completed so
   * that the user application can perform any required action.
   *
   * Depending on the return value of this function, the comm_do_irq_send()
   * function will send the next byte on the internal buffer (0x01) or the byte
   * returned by this function (0x00).
   */
  unsigned char (*irq_send_cb)(void *data, unsigned char *byte);
  /**
   * \brief Callback function to handle the end of interrupt driven receptions
   *
   * This function is called whenever an IRQ driven reception is completed so
   * that the user application can perform any required action.
   */
  unsigned char (*irq_receive_cb)(void *data, unsigned char byte);
  /**@}*/
  /* dma callbacks */
  /**
   * \defgroup callback_dma_pointers User DMA related function pointers
   *
   * This set of functions, that must be implemented by the user module that
   * uses the communication structure, are intended to handle the specific
   * requirements of each application using DMA driven communications.
   *
   * This function pointers must be assigned by the initialization function
   * of the user module.
   */
  /**@{*/
  /**
   * \brief Callback functions to handle the end of DMA driven transmissions
   *
   * This function is called whenever an DMA driven transmission is completed so
   * that the user application can perform any required action.
   */
  unsigned char (*dma_send_cb)(void *data);
  /**
   * \brief Callback functions to handle the end of DMA driven receptions
   *
   * This function is called whenever an DMA driven reception is completed so
   * that the user application can perform any required action.
   */
  unsigned char (*dma_receive_cb)(void *data);
  /**@}*/
} TComm;

/* public functions */
/**
 * \brief Function to initialize the communications structure
 *
 * This function initializes a TComm structure by default. All function
 * pointer are set to dummy functions that do nothing. These functions
 * have to be assigned to valid functions afterwards.
 *
 * This functions allows to enable or disable the use of the DMA. This
 * option can not be modified afterwards. The TTime structure, if provided,
 * will allow the communication device to use timeouts during reception.
 * Otherwise, the timeout feature will be disabled.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param use_dma flag to enable (0x01) or disable (0x00) the use of
 *        the DMA for the communications structure. Any other value is
 *        interpreted as an enable.
 * \param time a pointer to an initialized TTime structure to enable the
 *        use of timeouts. If timeouts are not desired or required, this
 *        parameter must be set to NULL (0x00000000).
 */
void comm_init(TComm *dev, unsigned char use_dma, TTime *time);
/**
 * \brief Function to start a blocking transmission
 *
 * This function is used to start a transmission in blocking mode. By default
 * the interrupt driven transmission is used, unless the use of the DMA has been
 * enabled in the call to the comm_init() function, in which case the DMA driven
 * transmission is used.
 *
 * This function blocks until all bytes have been transmitted or there has been
 * an error.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param data pointer to a vector where the data to be transmitted is stored.
 *        Memory for this parameter must be pre-allocated before calling this
 *        function to avoid unexpected behavior.
 * \param length Number of bytes to be transmitted.
 *
 * \return the status of the transmission. The possible values are the elements
 *         in the comm_error enumeration.
 */
comm_error comm_send(TComm *dev, unsigned char *data,
                     unsigned short int length);
/**
 * \brief Function to start a blocking reception with a timeout
 *
 * This function is used to start a reception in blocking mode. By default
 * the interrupt driven reception is used, unless the use of the DMA has been
 * enabled in the call to the comm_init() function, in which case the DMA driven
 * reception is used.
 *
 * This function blocks until all bytes have been received, there has been an
 * error or the amount of time specified as timeout has elapsed.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param data pointer to a vector where the received data is to be stored.
 *        Memory for this parameter must be pre-allocated before calling this
 *        function to avoid unexpected behavior.
 * \param length Number of bytes to be received.
 * \param timeout maximum time in micro-seconds to wait for the desired amount
 *        of bytes. If the TTime structure is not valid this parameter is
 * ignored.
 *
 * \return the status of the transmission. The possible values are the elements
 *         in the comm_error enumeration.
 */
comm_error comm_receive(TComm *dev, unsigned char *data,
                        unsigned short int *length, unsigned long int timeout);
/* IRQ functions */
/**
 * \brief Function to start an interrupt driven transmission
 *
 * This function is used to start a transmission in IRQ driven mode. This
 * function starts sending the data but returns immediately, reporting an error,
 * if any. The data in the input parameter is copied to the internal buffer
 * before it returns.
 *
 * This function calls the function of the low level communication module
 * pointed by the send_irq parameter.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param data pointer to a vector where the data to be transmitted is stored.
 *        Memory for this parameter must be pre-allocated before calling this
 *        function to avoid unexpected behavior.
 * \param length Number of bytes to be transmitted.
 *
 * \return the status of the transmission. The possible values are the elements
 *         in the comm_error enumeration.
 */
comm_error comm_send_irq(TComm *dev, unsigned char *data,
                         unsigned short int length);
/**
 * \brief Function to handle the end of the transmission of a single byte
 *
 * This function should be called from the interrupt handler each time a byte
 * has been sent. If this function is called as a result of the last byte of a
 * DMA transfer being sent, it calls the corresponding DMA transmission callback
 * function.
 *
 * If this function is called as a result of a byte being sent in an IRQ driven
 * transfer, this function calls the corresponding IRQ transmission callback
 * function. When the last byte has been sent, this function automatically stops
 * the current transmission, and returns a value of 0x00 to report it.
 *
 * The interrupt handler should use the value returned by this function to send
 * the next byte or disable the transmission interrupts when the transmission
 * is finished (in either case).
 *
 * Use the comm_get_error() function to get any possible error that happen
 * during transmission.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param byte this parameter holds the value to be sent next when the function
 *        returns 0x00, when it is part of an IRQ driven transmission. Otherwise
 *        this parameter must be ignored.
 *
 * \return 0x00 when the transmission has ended and any other value when there
 *         are still some bytes to be sent.
 */
unsigned char comm_do_irq_send(TComm *dev, unsigned char *byte);
/**
 * \brief Function to start an interrupt driven reception
 *
 * This function is used to start a reception in IRQ driven mode. This function
 * starts the reception process but returns immediately, reporting an error, if
 any.
 * The received data is stored into the internal buffer, and the
 comm_get_received_data()
 * function should be used to get it.
 *
 * This function calls the function of the low level communication module
 pointed
 * by the receive_irq parameter.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param length Number of bytes to be transmitted.
 *
 * \return the status of the reception. The possible values are the elements
 *         in the comm_error enumeration.

 */
comm_error comm_receive_irq(TComm *dev, unsigned short int length);
/**
 * \brief Function to handle the reception of a single byte
 *
 * This function should be called from the interrupt handler each time a byte
 * has been received. When this function is called, it first call the user
 * defined function pointed by the irq_receive_cb parameter to check the
 * received data (if necessary).
 *
 * If the callback function returns 0x00, the received byte is ignored and
 * the reception process continues. If the callback function returns any other
 * value, the new byte is stored into the internal buffer until all the desired
 * bytes have been received.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param byte this parameter holds the last byte that has been received through
 *        the communication device, and it is passed to the user callback in
 * order to be processed.
 *
 * \return 0x00 when the reception has ended and any other value when there
 *         are still some bytes to be received.
 */
unsigned char comm_do_irq_receive(TComm *dev, unsigned char byte);
/**
 * \brief Function to get the received data
 *
 * This function is used to get the data received by an IRQ driven reception.
 * This function can only be called when the reception has ended, otherwise it
 * reports a busy error.
 *
 * Use the comm_get_error() function to get any possible error that happen
 * during reception.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param data buffer where the received data will be copied to be used by the
 *        user application. The memory for this vector must be pre-allocated
 *        before calling this function.
 * \param length is the number of data actually received. In case of no error,
 *        this value must coincide with the desired value, but it can be less
 *        in case of any error.
 *
 * \return COMM_SUCCESS in case the reception process has ended, and COMM_BUSY
 *         otherwise.
 */
comm_error comm_get_received_data(TComm *dev, unsigned char *data,
                                  unsigned short int *length);
/**
 * \brief Function to cancel the current interrupt driven reception
 *
 * This function cancels any active IRQ driven reception. This function calls
 * the function of the low level communication module pointed by the
 * cancel_receive_irq parameter to actually stop and disable the reception
 * interrupts.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 */
void comm_cancel_irq_receive(TComm *dev);
/* DMA functions */
/**
 * \brief Function to start a DMA driven transmission
 *
 * This function is used to start a transmission using DMA. This function calls
 * the low level communication module pointed by the send_dma parameter. This
 * function should configure the DMA transaction with the provided information
 * and start it.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param data pointer to a vector where the data to be transmitted is stored.
 *        Memory for this parameter must be pre-allocated before calling this
 *        function to avoid unexpected behavior.
 * \param length Number of bytes to be transmitted.
 *
 * \return COMM_SUCCESS if the transmission has started successfully or
 * COMM_BUSY if there is an operation in progress.
 */
comm_error comm_send_dma(TComm *dev, unsigned char *data,
                         unsigned short int length);
/**
 * \brief Function to handle the transmission of all bytes
 *
 * This function is called when the last data byte has been sent to the
 * communication device. This function enables the TX interrupts to actually
 * wait until the last byte has been sent to avoid possible errors.
 *
 * It is not until the last byte has been actually sent, that the user
 * application callback function pointed by the dma_send_cb parameter is called.
 * This function should handle any required action after the transmission.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 */
void comm_do_dma_send(TComm *dev);
/**
 * \brief Function to start a DMA driven reception
 *
 * This function is used to start a reception using DMA. This function calls the
 * low level communication module pointed by the receive_dma parameter. This
 * function should configure the DMA transaction with the provided information
 * and start it.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param data pointer to a vector where the received data will be stored.
 *        Memory for this parameter must be pre-allocated before calling this
 *        function to avoid unexpected behavior. The contents of this vector
 *        are not valid until the operation has ended.
 * \param length Number of bytes to be received.
 *
 * \return COMM_SUCCESS if the reception has started successfully or COMM_BUSY
 *         if there is an operation in progress.
 */
comm_error comm_receive_dma(TComm *dev, unsigned char *data,
                            unsigned short int length);
/**
 * \brief Function to handle the reception of all desired bytes
 *
 * This function is called when the last data byte has been received through
 * the communication device. At this point, the user application callback
 * function pointed by the dma_receive_cb parameter is called. This function
 * should handle any required action after the reception.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 */
void comm_do_dma_receive(TComm *dev);
/**
 * \brief Function to cancel the current DMA driven reception
 *
 * This function is used to cancel any active DMA driven reception by calling
 * the low level communication device pointed by cancel_receive_dma. When this
 * function is called, the contents of the data vector passed to the
 * comm_send_dma() function can not be used.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 */
void comm_cancel_dma_receive(TComm *dev);
/* common functions */
/**
 * \brief Function to check whether the current transmission has ended or not
 *
 * This function is used to check whether there is an active transmission or
 * not. In the case the transmission has ended, this function indicates if
 * there has been an error or not. In case of an error, use the comm_get_error()
 * function to get it.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 *
 * \return COMM_BUSY if the transmission is still active, COMM_ERROR if the
 *         last transmission has ended with an error or COMM_SUCCESS if it
 *         ended successfully.
 */
comm_error comm_is_send_done(TComm *dev);
/**
 * \brief Function to check whether the current reception has ended or not
 *
 * This function is used to check whether there is an active reception or
 * not. In the case the reception has ended, this function indicates if
 * there has been an error or not. In case of an error, use the comm_get_error()
 * function to get it.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 *
 * \return COMM_BUSY if the reception is still active, COMM_ERROR if the
 *         last reception has ended with an error or COMM_SUCCESS if it
 *         ended successfully.
 */
comm_error comm_is_receive_done(TComm *dev);
/**
 * \brief Function to get the communication error
 *
 * This function returns the error of the last transmission or reception
 * operation.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 *
 * \return the error of the last operation or COMM_BUSY if the operation
 *         is still in progress.
 */
static inline unsigned char comm_get_error(TComm *dev) { return dev->error; }

/**
 * \brief Function to set the communication error
 *
 * The user application and the low level communications module can use this
 * function to set the internal communication error at any time. The error
 * set by this function can be get with the comm_get_error() function.
 *
 * \param dev pointer to a valid TComm structure to be initialized. If
 *        memory is not pre-allocated before calling this function, its
 *        behavior is unpredictable.
 * \param error identifier of the error to be set
 */
static inline void comm_set_error(TComm *dev, unsigned char error) {
  dev->error = error;
}

#ifdef __cplusplus
}
#endif

#endif
