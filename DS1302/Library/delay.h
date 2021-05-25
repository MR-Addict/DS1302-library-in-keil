void delay_us(uint8_t us_count)
{
   while (us_count != 0)
   {
      us_count--;
   }
}

void delay_ms(uint8_t ms_count)
{
   while (ms_count != 0)
   {
      delay_us(112);
      ms_count--;
   }
}

void delay_sec(uint8_t sec_count)
{

   while (sec_count != 0)
   {
      delay_ms(1000);
      sec_count--;
   }
}