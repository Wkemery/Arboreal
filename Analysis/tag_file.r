startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/tag_file_time.txt", header = TRUE)


startup<-startup[startup$time < 0.15,]

hist(log(startup$numtags))
plot(time~files, data = startup, main= "TagFile: Time ~ Files")
plot(time~tags, data = startup, main = "TagFile: Time ~ Tags")
plot(time~numtags, data = startup)


fit<-lm(time~numtags + I(numtags^2), data = startup)
summary(fit)
plot(fit)


plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="Files", main="Files Residual Plot")


plot(time~numtags, data = startup, main = "TagFile: Time ~ Numtags")

round(fit$coefficients, 6)
c<-fit$coefficients
cbind(c, seq(3))
summary(fit)

numtagsHat<-seq(0,14, length=1000)
timeHat<-c[1] + c[2]*numtagsHat + c[3]*numtagsHat^2
lines(timeHat~numtagsHat, col="red", lwd = 3)
