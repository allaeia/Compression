function I=image()
    nom_fichier = uigetfile();
    nom_fichier=string(nom_fichier);
    raccourci = strchr(nom_fichier,'.');
    n = length(raccourci);
    fin = part(raccourci,2:n)
    
    existence = fileinfo(nom_fichier);
    if(existence <> [])
        if((fin=="png" | fin=="bmp"))
            I=imread(nom_fichier);
            disp("image correctement chargee");
//            imshow(I);
        else
            err = messagebox("veuillez choisir une image avec le bon format (png ou bmp)", "modal", "info", ["ok" "abandon"]);
            if(err==1)
                I=image();
            else
                abort;
            end
        end
    else
        err = messagebox("veuillez choisir une image qui existe", "modal", "info", ["ok" "abandon"]);
        if(err==1)
            I=image();
        else
            abort;
        end
    end
    //pour info, les jpeg ont l'air trop grosses...
endfunction

function I=load()
    r = messagebox("Quel type d image voulez vous traiter? (png ou bmp)", "modal", "info", ["webcam" "fichier"]);
    
    if(r==2)
       I=image()
    else
        //a tester si ça marche avec une webcam USB...
        n = camopen();
        if( isdef("n") & n>0 )
            //for idx=1:100,
                I=avireadframe(n);
                imshow(I);
                I=resizewebcam(I,300);
                imshow(I);
            //end
        end
        avilistopened()
        aviclose(n);
    end
endfunction
