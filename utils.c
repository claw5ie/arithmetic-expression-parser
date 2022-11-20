int
powi(int x, int e)
{
  if (e < 0)
    return 0;
  else if (e == 0)
    return 1;

  int res = powi(x, e / 2);

  res *= res;

  if (e % 2 == 0)
    return res;
  else
    return x * res;
}
