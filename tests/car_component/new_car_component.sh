
if [ ! -d ./.dummy ]; then
  echo "execute this script inside of tests/car_component"
  exit 1
fi

if [ $# -ne 1 ] ; then
  echo "usage ${0} [component name]"
  exit 2
fi

if [ -d $1 ]; then
  echo "component already present"
  exit 3
fi

new_component=${1}

cp -r ./.dummy $new_component

mv $new_component/dummy.h $new_component/$new_component.h
mv $new_component/dummy.c $new_component/$new_component.c

comp_h=$new_component/$new_component.h
comp_c=$new_component/$new_component.c

sed -i "s/dummy/$new_component/g" $comp_h
sed -i "s/Dummy/$new_component/g" $comp_h

sed -i "s/dummy/$new_component/g" $comp_c
sed -i "s/Dummy/$new_component/g" $comp_c

