#include "Comm.h"

/* private functions */
/* dummy functions for all function pointers */
unsigned char dummy_send_irq(unsigned char first_byte) { return 0x00; }

unsigned char dummy_enable_tx_irq(void) { return 0x00; }

unsigned char dummy_receive_irq(void) { return 0x00; }

unsigned char dummy_cancel_receive_irq(void) { return 0x00; }

/* DMA functions */
unsigned char dummy_send_dma(unsigned char *data, unsigned short int length) {
  return 0x00;
}

unsigned char dummy_receive_dma(unsigned char *data,
                                unsigned short int length) {
  return 0x00;
}

unsigned char dummy_cancel_receive_dma(void) { return 0x00; }

/* irq callbacks */
unsigned char dummy_irq_send_cb(void *data, unsigned char *byte) {
  return 0x01;
}

unsigned char dummy_irq_receive_cb(void *data, unsigned char byte) {
  return 0x01;
}

/* dma callbacks */
unsigned char dummy_dma_send_cb(void *data) { return 0x00; }

unsigned char dummy_dma_receive_cb(void *data) { return 0x00; }

/* public functions */
void comm_init(TComm *dev, unsigned char use_dma, TTime *time) {
  buffer_init(&dev->tx_buffer);
  dev->irq_sending = 0x00;
  dev->dma_sending = 0x00;
  dev->tx_length = 0;
  buffer_init(&dev->rx_buffer);
  dev->irq_receiving = 0x00;
  dev->dma_receiving = 0x00;
  dev->rx_length = 0;
  dev->use_dma = use_dma;
  dev->dma_waiting_buffer_empty = 0x00;
  dev->error = 0x00;
  dev->time = time;
  dev->data = 0x00000000;
  dev->send_irq = dummy_send_irq;
  dev->enable_tx_irq = dummy_enable_tx_irq;
  dev->receive_irq = dummy_receive_irq;
  dev->cancel_receive_irq = dummy_cancel_receive_irq;
  dev->send_dma = dummy_send_dma;
  dev->receive_dma = dummy_receive_dma;
  dev->cancel_receive_dma = dummy_cancel_receive_dma;
  dev->irq_send_cb = dummy_irq_send_cb;
  dev->irq_receive_cb = dummy_irq_receive_cb;
  dev->dma_send_cb = dummy_dma_send_cb;
  dev->dma_receive_cb = dummy_dma_receive_cb;
}

comm_error comm_send(TComm *dev, unsigned char *data,
                     unsigned short int length) {
  comm_error error;

  if (dev->use_dma) // send data using DMA
    error = comm_send_dma(dev, data, length);
  else // send data using interrupts
    error = comm_send_irq(dev, data, length);
  // wait until transmission is complete
  while (dev->irq_sending || dev->dma_sending)
    ;

  return error;
}

comm_error comm_receive(TComm *dev, unsigned char *data,
                        unsigned short int *length,
                        unsigned long int timeout_us) {
  comm_error error;

  // initialize timeout
  if (dev->time != 0x00000000)
    time_set_timeout(dev->time, timeout_us);
  // start reception
  if (dev->use_dma) // receive data using DMA
    error = comm_receive_dma(dev, data, *length);
  else // receive data using interrupts
    error = comm_receive_irq(dev, *length);
  // wait for the reception to end or timeout
  while (dev->irq_receiving || dev->dma_receiving) {
    if (dev->time != 0x00000000 && time_is_timeout(dev->time)) {
      error = COMM_TIMEOUT;
      // cancel current reception
      if (dev->use_dma)
        comm_cancel_dma_receive(dev);
      else
        comm_cancel_irq_receive(dev);
      *length = 0;
      return error;
    }
  }
  // get the received data
  if (dev->use_dma)
    *length = buffer_get_num_data(&dev->rx_buffer);
  else
    comm_get_received_data(dev, data, length);
  // get any possible error
  error = (comm_error)comm_get_error(dev);

  return error;
}

/* IRQ functions */
comm_error comm_send_irq(TComm *dev, unsigned char *data,
                         unsigned short int length) {
  if (dev->irq_sending || dev->dma_sending)
    return COMM_BUSY;
  else {
    // copy data to the tx_buffer
    if (length > 1) {
      if (buffer_write(&dev->tx_buffer, &data[1], length - 1) < length - 1) {
        buffer_flush(&dev->tx_buffer);
        return COMM_FULL;
      }
    }
    dev->tx_length = length;
    dev->irq_sending = 0x01;
    // start transmission
    dev->send_irq(data[0]); // TO DO: check return value
    return COMM_SUCCESS;
  }
}

unsigned char comm_do_irq_send(TComm *dev, unsigned char *byte) {
  if (dev->dma_waiting_buffer_empty) {
    dev->tx_length--;
    if (dev->tx_length == 0) // transmission done
    {
      dev->dma_waiting_buffer_empty = 0x00;
      dev->irq_sending = 0;
      // call the user function
      dev->dma_send_cb(dev->data);
      return 0x00;
    } else
      return 0x02;
  } else if (dev->irq_send_cb(dev->data,
                              byte)) // send the byte in the buffer, not the one
                                     // returned by the function
  {
    // read the next byte from the buffer
    dev->tx_length--;
    if (dev->tx_length == 0) // transmission done
    {
      dev->irq_sending = 0;
      return 0x00;
    } else // bytes remaining to be send
    {
      buffer_read_byte(&dev->tx_buffer, byte);
      return 0x01;
    }
  } else // send the byte returned by the function
    return 0x01;
}

comm_error comm_receive_irq(TComm *dev, unsigned short int length) {
  if (dev->irq_receiving || dev->dma_receiving)
    return COMM_BUSY;
  else {
    dev->rx_length = length;
    dev->irq_receiving = 0x01;
    // start reception
    dev->receive_irq(); // TO DO: check return value
    return COMM_SUCCESS;
  }
}

unsigned char comm_do_irq_receive(TComm *dev, unsigned char byte) {
  if (dev->irq_receive_cb(dev->data, byte)) // store the byte received
  {
    buffer_write_byte(&dev->rx_buffer, byte); // move data into the buffer
    if (dev->rx_length != -1) {
      dev->rx_length--;
      if (dev->rx_length == 0) /* reception done */
      {
        dev->irq_receiving = 0x00;
        return 0x00;
      } else
        return 0x01;
    } else
      return 0x01;
  } else // ignore the byte received
    return 0x01;
}

void comm_cancel_irq_receive(TComm *dev) {
  dev->irq_receiving = 0x00;
  dev->rx_length = 0;
  dev->cancel_receive_irq();
}

comm_error comm_get_received_data(TComm *dev, unsigned char *data,
                                  unsigned short int *length) {
  if (dev->irq_receiving || dev->dma_receiving)
    return COMM_BUSY;
  else {
    *length = buffer_get_num_data(&dev->rx_buffer);
    buffer_read(&dev->rx_buffer, data, *length);
    return COMM_SUCCESS;
  }
}

/* DMA functions */
comm_error comm_send_dma(TComm *dev, unsigned char *data,
                         unsigned short int length) {
  if (dev->irq_sending || dev->dma_sending)
    return COMM_BUSY;
  else {
    dev->dma_sending = 0x01;
    dev->tx_length = 1;
    // start transmission
    dev->send_dma(data, length); // TO DO: check the return value
    return COMM_SUCCESS;
  }
}

void comm_do_dma_send(TComm *dev) {
  dev->dma_waiting_buffer_empty = 0x01;
  dev->irq_sending = 0x00;
  dev->dma_sending = 0x00;
  dev->enable_tx_irq();
}

comm_error comm_receive_dma(TComm *dev, unsigned char *data,
                            unsigned short int length) {
  if (dev->irq_receiving || dev->dma_receiving)
    return COMM_BUSY;
  else {
    dev->dma_receiving = 0x01;
    // start reception
    dev->receive_dma(data, length);
    return COMM_SUCCESS;
  }
}

void comm_do_dma_receive(TComm *dev) {
  dev->dma_receiving = 0x00;
  // call the user function
  dev->dma_receive_cb(dev->data);
}

void comm_cancel_dma_receive(TComm *dev) {
  dev->dma_receiving = 0x00;
  dev->cancel_receive_dma();
}

/* common functions */
comm_error comm_is_send_done(TComm *dev) {
  if (dev->irq_sending || dev->dma_sending)
    return COMM_BUSY;
  else {
    if (dev->error == 0x00)
      return COMM_SUCCESS;
    else
      return COMM_ERROR;
  }
}

comm_error comm_is_receive_done(TComm *dev) {
  if (dev->irq_receiving || dev->dma_receiving)
    return COMM_BUSY;
  else {
    if (dev->error == 0x00)
      return COMM_SUCCESS;
    else
      return COMM_ERROR;
  }
}
