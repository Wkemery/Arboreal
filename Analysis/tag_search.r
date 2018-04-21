startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/tag_search_time.txt", header = TRUE)


startup<-startup[startup$numfiles > 0,]
hist(log(startup$time), main="Histogram of tag_search times", xlab="Enemy Casualties")


startup<-startup[startup$time < 0.15,]
startup$numtags <- as.factor(startup$numtags)
startup$numfiles<-log(startup$numfiles)

hist((startup$numtags))
#startup$numtags <- factor(startup$numtags) 
plot(time~files, data = startup)
plot(time~tags, data = startup, col = numtags)
plot(time~numtags, data = startup)
plot(time~numfiles, data = startup, col=numtags)

fit<-lm(time~numtags*numfiles, data = startup)

summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$numfiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")

plot(time~numtags, data = startup)
plot(time~numfiles, data = startup)

c<-fit$coefficients
cbind(c, seq(6))
summary(fit)

numtagsHat<-seq(0,14, length=1000)
numtagsHat<- 1
numtagsHat<- 5
numfilesHat<-seq(0,695,length=1000)
numfilesHat<- 0
numfilesHat<- mean(startup$numfiles)
numfilesHat<- 695
  
timeHat<-c[1] + c[2]*numtagsHat + c[3]*numfilesHat + c[4]*numtagsHat^2  + c[5]*log(numtagsHat) + c[6]*numtagsHat*numfilesHat  
lines(timeHat~numtagsHat, col="red", lwd = 3)
