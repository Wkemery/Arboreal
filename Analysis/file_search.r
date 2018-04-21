library(MASS)
startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/file_search_time.txt", header = TRUE)

startup<-startup[startup$time < 0.2,]
plot(time~files, data = startup, col = numfiles)
plot(time~tags, data = startup, col = numfiles)
plot(time~numfiles, data = startup)


b<-boxcox(time~numfiles, data = startup)
(lambda <- b$x[which.max(b$y)])
fit <- lm(((time^lambda-1)/lambda) ~numfiles, data = startup)


fit<-lm(time~numfiles, data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Tags", main="Files Residual Plot")
plot(fit$residuals~startup$numfiles,ylab="Residuals", xlab="numfiles", main="numfiles Residual Plot")
plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")


plot(time~files, data = startup,main = "")
plot(time~tags, data = startup)
plot(time~numfiles, data = startup)

round(fit$coefficients, 6)
c<-fit$coefficients
cbind(c, seq(4))
summary(fit)

numfilesHat<-seq(0,600, length=1000)
timeHat<-c[1] + c[2]*numfilesHat
lines(timeHat~numfilesHat, col="red", lwd = 3)


legend("bottomleft", c("Temp=-120", "Temp=50", "Temp=195"), col=c("red", "blue", "green"), lty=1, lwd = 3)
