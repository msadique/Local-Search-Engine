for i in `find -name "file*.txt"`
do 
cvs add $i
cvs ci -m "Added this file" $i
done

