startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataNormal/tag_search_time.txt", header = TRUE)


#startup<-startup[startup$foundfiles > 0,]
hist(log(startup$time), main="Histogram of tag_search times", xlab="Enemy Casualties")



#startup$numtags <- factor(startup$numtags) 
plot(log(time)~files, data = startup)
plot(log(time)~log(tags), data = startup)
plot(log(time)~numtags, data = startup)
plot(log(time)~foundfiles, data = startup, col=numtags)

fit<-lm(log(time)~files*tags*numtags*foundfiles + foundfiles:numtags + I(files^2) 
        + I(foundfiles^2)+ I(tags^2)+I(files^3) + I(foundfiles^3) 
        + I(foundfiles^4) - files:tags:numtags:foundfiles - files:tags:foundfiles
        - numtags:foundfiles - tags:numtags - files:numtags:foundfiles - files:foundfiles
        - files:tags:numtags - files:numtags, data = startup)

fit<-lm(log(time)~ files + tags + numtags + foundfiles + I(files^2) + I(foundfiles^2) 
        + I(tags^2) + I(files^3)+ files:tags + 
          tags:foundfiles + tags:numtags:foundfiles, data = startup)

summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$foundfiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")

